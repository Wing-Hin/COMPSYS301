#include "navigation.h"
#include <assert.h>
#include <stdio.h>

static void test_headings(void)
{
    const uint8_t map[9] = {0};
    const BfsCell neighbours[] = {{0,1}, {1,2}, {2,1}, {1,0}};
    const NavAction actions[] = {NAV_FORWARD, NAV_RIGHT, NAV_UTURN, NAV_LEFT};
    unsigned int heading, desired;
    for (heading = 0; heading < 4; ++heading) {
        for (desired = 0; desired < 4; ++desired) {
            Navigation nav = {0};
            BfsCell path[] = {{1,1}, neighbours[desired]};
            assert(NavigationStart(&nav, map, 3, 3, path, 2, (NavHeading)heading));
            assert(nav.action == actions[(desired + 4 - heading) % 4]);
            if (heading != desired) {
                assert(NavigationTurnStarted(&nav));
                assert(!NavigationTurnStarted(&nav));
                assert(nav.action == NAV_WAIT && nav.heading == (NavHeading)heading);
                assert(NavigationTurnFinished(&nav, true));
                assert(nav.heading == (NavHeading)desired && nav.index == 0);
            }
            assert(NavigationArrive(&nav, 1));
            assert(nav.state == NAV_DONE && nav.action == NAV_STOP);
            assert(NavigationArrive(&nav, 1));
        }
    }
}

static void test_landmarks(void)
{
    const uint8_t map[] = {1,1,0,1,1, 0,0,0,0,0, 1,1,1,1,1};
    const BfsCell path[] = {{1,0},{1,1},{1,2},{1,3},{1,4}};
    Navigation nav = {0};
    assert(NavigationStart(&nav, map, 3, 5, path, 5, NAV_EAST));
    assert(nav.next_index == 2); /* Skip corridor cell 1; stop at side branch. */
    assert(NavigationArrive(&nav, 2));
    assert(nav.action == NAV_FORWARD && nav.next_index == 4);
    assert(NavigationArrive(&nav, 2)); /* Same intersection doesn't advance twice. */
    assert(nav.index == 2 && nav.next_index == 4);
    assert(NavigationArrive(&nav, 4));
    assert(nav.state == NAV_DONE);

    assert(NavigationStart(&nav, map, 3, 5, path, 5, NAV_EAST));
    assert(!NavigationArrive(&nav, 4)); /* Must not skip the junction. */
    assert(nav.state == NAV_FAULT && nav.action == NAV_STOP);
}

static void test_bfs_route(void)
{
    const uint8_t map[] = {1,1,0, 0,0,0};
    BfsWorkspace work;
    BfsCell path[BFS_MAX_CELLS];
    uint16_t length;
    Navigation nav = {0};
    assert(BfsFindPath(map, 2, 3, (BfsCell){1,0}, (BfsCell){0,2},
                       &work, path, BFS_MAX_CELLS, &length) == BFS_FOUND);
    assert(NavigationStart(&nav, map, 2, 3, path, length, NAV_EAST));
    assert(nav.next_index == 2);
    assert(NavigationArrive(&nav, 2));
    assert(nav.action == NAV_LEFT && nav.heading == NAV_EAST);
    assert(NavigationTurnStarted(&nav));
    assert(NavigationTurnFinished(&nav, true));
    assert(nav.heading == NAV_NORTH && nav.next_index == 3);
    assert(NavigationArrive(&nav, 3));
    assert(nav.state == NAV_DONE);
}

static void test_invalid_and_faults(void)
{
    const uint8_t map[] = {0,0,0,1};
    const BfsCell bad[] = {{0,0},{1,1}};
    const BfsCell path[] = {{0,0},{0,1}};
    Navigation nav = {0};
    assert(!NavigationStart(&nav, map, 2, 2, bad, 2, NAV_NORTH));
    assert(!NavigationStart(&nav, map, 2, 2, path, 0, NAV_NORTH));
    assert(!NavigationStart(&nav, map, 2, 2, path, 2, (NavHeading)4));
    assert(NavigationStart(&nav, map, 2, 2, path, 1, NAV_NORTH));
    assert(nav.state == NAV_DONE);
    assert(NavigationStart(&nav, map, 2, 2, path, 2, NAV_NORTH));
    assert(NavigationTurnStarted(&nav));
    assert(!NavigationTurnFinished(&nav, false));
    assert(nav.state == NAV_FAULT && nav.heading == NAV_NORTH);
    assert(!NavigationArrive(&nav, 1));
}

int main(void)
{
    test_headings(); test_landmarks(); test_bfs_route(); test_invalid_and_faults();
    puts("PASS: navigation headings, BFS route, landmarks, duplicate events and faults");
    return 0;
}
