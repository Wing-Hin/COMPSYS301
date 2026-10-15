#include "robot_navigation.h"
#include "line_follow.h"
/* Use the same headers/config as the compiled turn.c, not duplicate copies. */
#include "../Lucas's turning code/turn.h"
#include "../Lucas's turning code/turn_hardware.h"
#include <stddef.h>

#define LANDMARK_CONFIRM_FRAMES 3U

static Navigation nav;
static BfsWorkspace search;
static BfsCell route[BFS_MAX_CELLS];
static const RobotNavigationConfig *settings;
static RobotNavigationError error;
static int16_t previousLeft, previousRight;
static int32_t leftTravel, rightTravel;
static uint32_t segmentStarted, lastFresh, lastTurnUpdate, turnStarted;
static uint8_t hitFrames, clearFrames;
static bool intersectionArmed;

static int32_t Delta(int16_t current, int16_t previous)
{
    int32_t delta = (int32_t)current - previous;
    if (delta > 32767L) delta -= 65536L;
    if (delta < -32768L) delta += 65536L;
    return delta;
}

static void BeginSegment(uint32_t now, bool afterTurn)
{
    previousLeft = QuadDec_M1_GetCounter();
    previousRight = QuadDec_M2_GetCounter();
    leftTravel = rightTravel = afterTurn ? settings->turn_exit_offset : 0;
    segmentStarted = now;
    hitFrames = clearFrames = 0;
    intersectionArmed = false;
}

static void Fail(RobotNavigationError reason)
{
    error = reason;
    NavigationFail(&nav);
    TurnAbort();
    SetMotorSpeed(0, 0);
}

bool RobotNavigationInit(const RobotNavigationConfig *config, uint32_t now)
{
    uint16_t length = 0;
    BfsStatus result;
    TurnAbort();
    TurnReset();
    SetMotorSpeed(0, 0);
    nav = (Navigation){0};
    error = ROBOT_NAV_OK;
    settings = config;
    if (config == NULL || config->cell_counts == 0 ||
        config->cell_counts > 1000000UL ||
        config->arrival_window >= (config->cell_counts + 1) / 2 ||
        config->turn_exit_offset <= -(int32_t)config->cell_counts ||
        config->turn_exit_offset >= (int32_t)config->cell_counts ||
        config->segment_timeout_ms == 0 || config->segment_timeout_ms > 600000UL ||
        (config->left_encoder_sign != 1 && config->left_encoder_sign != -1) ||
        (config->right_encoder_sign != 1 && config->right_encoder_sign != -1) ||
        config->speed == 0 || config->speed > 99) {
        Fail(ROBOT_NAV_CONFIG);
        return false;
    }
    result = BfsFindPath(config->map, config->rows, config->cols,
                         config->start, config->goal, &search,
                         route, BFS_MAX_CELLS, &length);
    if (result != BFS_FOUND) {
        Fail(result == BFS_NO_PATH ? ROBOT_NAV_NO_PATH : ROBOT_NAV_CONFIG);
        return false;
    }
    if (!NavigationStart(&nav, config->map, config->rows, config->cols,
                         route, length, config->heading)) {
        Fail(ROBOT_NAV_CONFIG);
        return false;
    }
    /* Existing TurnStart includes forward approach to a junction. It cannot
     * safely align an arbitrary starting pose, so require initial alignment. */
    if (nav.state == NAV_TURN_REQUESTED) {
        Fail(ROBOT_NAV_INITIAL_HEADING);
        return false;
    }
    BeginSegment(now, false);
    lastFresh = now;
    return true;
}

static void Arrive(uint32_t now, bool leftBranch, bool rightBranch)
{
    if (!NavigationArrive(&nav, nav.next_index)) {
        Fail(ROBOT_NAV_POSITION);
        return;
    }
    if (nav.state == NAV_TURN_REQUESTED) {
        if (nav.action == NAV_UTURN) { Fail(ROBOT_NAV_UTURN); return; }
        if ((nav.action == NAV_LEFT && !leftBranch) ||
            (nav.action == NAV_RIGHT && !rightBranch)) {
            Fail(ROBOT_NAV_BRANCH);
            return;
        }
        if (!TurnStart(nav.action == NAV_LEFT ? TURN_LEFT : TURN_RIGHT)) {
            Fail(ROBOT_NAV_TURN);
            return;
        }
        NavigationTurnStarted(&nav);
        lastTurnUpdate = turnStarted = now;
    } else if (nav.state == NAV_DONE) {
        SetMotorSpeed(0, 0);
    } else {
        /* A straight-through junction is still one confirmed landmark. */
        BeginSegment(now, false);
    }
}

void RobotNavigationUpdate(const SensorFrame *frame, uint32_t now)
{
    int16_t left, right;
    int32_t target, lower, upper;
    bool leftBranch, rightBranch;
    unsigned int i;
    if (nav.state == NAV_IDLE || nav.state == NAV_DONE || nav.state == NAV_FAULT) {
        SetMotorSpeed(0, 0);
        return;
    }
    if (frame != NULL && frame->fresh) {
        for (i = 0; i < N_SENSORS; ++i) {
            if (frame->state[i] != SENSOR_BLACK && frame->state[i] != SENSOR_WHITE) {
                Fail(ROBOT_NAV_SENSOR);
                return;
            }
        }
        lastFresh = now;
        controlSensorFrame = *frame;
    } else if ((uint32_t)(now - lastFresh) >= LINE_FOLLOW_SENSOR_TIMEOUT_MS) {
        Fail(ROBOT_NAV_SENSOR);
        return;
    }

    if (nav.state == NAV_TURNING) {
        TurnState result;
        if ((uint32_t)(now - turnStarted) >= TURN_TIMEOUT_MS) {
            Fail(ROBOT_NAV_TIMEOUT);
            return;
        }
        /* No catch-up loop over repeated samples. Wall-clock timeout above
         * remains accurate if ADC updates are slower than TURN_UPDATE_MS. */
        if (frame == NULL || !frame->fresh ||
            (uint32_t)(now - lastTurnUpdate) < TURN_UPDATE_MS) return;
        lastTurnUpdate = now;
        result = TurnUpdate();
        if (result == TURN_FAULT) { Fail(ROBOT_NAV_TURN); return; }
        if (result == TURN_DONE) {
            NavigationTurnFinished(&nav, true);
            TurnReset();
            BeginSegment(now, true);
        }
        return; /* Never let the line follower overwrite an active turn. */
    }

    if ((uint32_t)(now - segmentStarted) >= settings->segment_timeout_ms) {
        Fail(ROBOT_NAV_TIMEOUT);
        return;
    }
    left = QuadDec_M1_GetCounter();
    right = QuadDec_M2_GetCounter();
    leftTravel += settings->left_encoder_sign * Delta(left, previousLeft);
    rightTravel += settings->right_encoder_sign * Delta(right, previousRight);
    previousLeft = left;
    previousRight = right;
    target = (int32_t)((nav.next_index - nav.index) * settings->cell_counts);
    lower = target - (int32_t)settings->arrival_window;
    upper = target + (int32_t)settings->arrival_window;

    /* The destination can be midway along a corridor: no intersection needed.
     * Require both wheels, so one spinning wheel cannot count as arrival. */
    if (nav.next_index == nav.length - 1) {
        if (leftTravel >= target && rightTravel >= target) {
            Arrive(now, false, false);
            return;
        }
    } else if (leftTravel > upper || rightTravel > upper) {
        Fail(ROBOT_NAV_POSITION); /* Expected landmark was missed. */
        return;
    }
    if (frame == NULL || !frame->fresh) return;
    leftBranch = frame->state[Q3] == SENSOR_BLACK;
    rightBranch = frame->state[Q4] == SENSOR_BLACK;
    if (!leftBranch && !rightBranch) {
        hitFrames = 0;
        if (clearFrames < LANDMARK_CONFIRM_FRAMES) ++clearFrames;
        if (clearFrames >= LANDMARK_CONFIRM_FRAMES) intersectionArmed = true;
    } else {
        clearFrames = 0;
        if (intersectionArmed && leftTravel >= lower && rightTravel >= lower &&
            nav.next_index != nav.length - 1) {
            if (++hitFrames >= LANDMARK_CONFIRM_FRAMES) {
                Arrive(now, leftBranch, rightBranch);
                if (nav.state != NAV_FOLLOWING) return;
            }
        } else hitFrames = 0;
    }
    straightFromFrame(settings->speed, frame);
}

const Navigation *RobotNavigationGet(void) { return &nav; }
RobotNavigationError RobotNavigationGetError(void) { return error; }
