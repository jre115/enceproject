#ifndef NAV_H
#define NAV_H

#include "system.h"
#include "pacer.h"
#include "navswitch.h"

/* Initialise the matrix lights.  */
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4
#define PUSH 5

/* Returns value of direction if the navswitch has been moved, otherwise returns 0*/
int8_t direction_moved(void); 
/* returns 1 if the direction pushed matches the desired direction*/

int8_t is_goal_nav(int8_t);

#endif //NAV_H