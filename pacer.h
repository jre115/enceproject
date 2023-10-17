/*
# File:   pacer.h
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  Contains logic for the pacer function. Custom function.
*/

#ifndef PACER_H
#define PACER_H

#include "system.h"

#define fcpu 8000000
#define prescaler 1024

/* Initialise the pacer module with specified frequency  */
void pacer_init (uint16_t pacer_frequency);


/*The function waits for the specified period using the pacer.
 It uses Timer/Counter 1 to generate a delay based on the pacer period.*/
void pacer_wait (void);

#endif //PACER_H
