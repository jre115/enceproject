#include <avr/io.h>
#include "button.h"


/** Returns non-zero if button pressed.  */
int button_pressed_p (void)
{
    return (PIND & (1 << 7));
}


/** Initialise button1.  */
void button_init (void)
{
    DDRD &= ~(1 << 7);
}

