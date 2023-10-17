/*
# File:   nav.h
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  contains logic for the navigation switch
*/

#ifndef NAV_H
#define NAV_H

#include "system.h"
#include "pacer.h"
#include "navswitch.h"

/* Initialise the navswitch directions.  */
#define NORTH '1'
#define EAST '2'
#define SOUTH '3'
#define WEST '4'
#define PUSH '5'
#define ANY '6'
#define NO_DIRECTION '0'

#define TRUE 1
#define FALSE 0


/* Returns value of direction if the navswitch has been moved, otherwise returns No Direction*/
char nav_direction_moved(void); 


/* returns True if the direction pushed matches the desired direction amd false if not*/
int8_t nav_is_goal(int8_t);

#endif //NAV_H