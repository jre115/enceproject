#include <avr/io.h>
#include "system.h"

int main (void)
{
    system_init ();
    
    /* Initialise port to drive LED 1.  */
    DDRC |= (1 << 2); // set pin 2 to output
    DDRD &= ~(1 << 7); // set bit 7 to be zero

    while (1)
    {
        /* Turn LED 1 on.  */
        if ((PIND & (1<<7)) != 0) {
            PORTC |= (1 << 2); // turn on LED
        } else {
            PORTC &= ~(1 << 2); // turn off LED
        }
    }
}
