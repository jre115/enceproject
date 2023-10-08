#include <avr/io.h>
#include "led.h"
#include "navswitch.h"


/** Turn LED1 on.  */
void led_on (void)
{
    PORTC |= (1 << 2);
}


/** Turn LED1 off.  */
void led_off (void)
{
    PORTC &= ~(1 << 2);
}


/** Initialise LED1.  */
void led_init (void)
{
    DDRC |= (1 << 2);
}

/** Turn blue led on when navswitched pressed*/
void led_run (void)
{
    while(1) {
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            led_on();
        } else {
            led_off();
        }
    }
}