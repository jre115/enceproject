/*
# File:   displays.c
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  Contains logic for the pacer function
*/

#include <avr/io.h>
#include "pacer.h"

static uint16_t pacer_period;


/* Initialise the pacer module with specified frequency  */
void pacer_init (uint16_t pacer_frequency)
{
    // Set Timer/Counter 1 Control Registers
    TCCR1A = 0x00;
    TCCR1B = 0x05;
    TCCR1C = 0x00;

    // Calculate delay and pacer period
    uint16_t delay = (((1 * 1000) / pacer_frequency) / 2); 
    pacer_period = (delay * (fcpu / prescaler) / 1000);
}

/*The function waits for the specified period using the pacer.
 It uses Timer/Counter 1 to generate a delay based on the pacer period.*/
void pacer_wait (void)
{
    while (TCNT1 < (pacer_period)) {
        continue;
    }
    // resets after the while loop
    TCNT1 = 0;
}
