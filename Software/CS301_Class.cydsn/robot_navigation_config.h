#ifndef ROBOT_NAVIGATION_CONFIG_H
#define ROBOT_NAVIGATION_CONFIG_H

#include "robot_navigation.h"

/* 1: planned route, 0: legacy branch-triggered turning for bench comparison. */
#ifndef ROBOT_USE_ROUTE_NAVIGATION
#define ROBOT_USE_ROUTE_NAVIGATION 1
#endif
extern const RobotNavigationConfig robotNavigationConfig;

#endif
