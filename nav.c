#include "navswitch.h"
#include "system.h"
#include "nav.h"
#define TRUE 1
#define FALSE 0
#define NO_DIRECTION 0

/* Returns value of direction if the navswitch has been moved, otherwise returns 0*/
int8_t direction_moved(void)
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

/* returns 1 if the direction pushed matches the desired direction*/

int8_t is_goal_nav(int8_t goal)
{
    int8_t direction = direction_moved();
    if (direction == goal) {
        return TRUE;
    } else {
        return FALSE;
    }
}