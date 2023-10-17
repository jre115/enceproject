/*
# File:   nav.c
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  contains logic for the navigation switch
*/

#include "system.h"
#include "navswitch.h"
#include "nav.h"


/* Returns value of direction if the navswitch has been moved, otherwise returns No Direction*/
char nav_direction_moved(void)
{
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        return NORTH;
    } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
        return EAST;
    } else if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        return SOUTH;
     }else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
        return WEST;
    } else if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        return PUSH;
    }

    return NO_DIRECTION;
}


/* returns True if the direction pushed matches the desired direction amd false if not*/
int8_t nav_is_goal(int8_t goal)
{
    int8_t direction = nav_direction_moved();
    if (direction == goal) {
        return TRUE;
    } else {
        return FALSE;
    }
}