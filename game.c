#include <avr/io.h>
#include <stdlib.h>
#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#include "matrix.h"
#define PACER_RATE  500/* Pacer loop - controls main loop */
#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2


void init_blue_led(void) 
{
    /* Initialise port to drive LED 1.  */
    DDRC |= (1 << 2); // set pin 2 to output
    DDRD &= ~(1 << 7); // set bit 7 to be zero
}

void blue_led(void)
{
    /* Turn LED 1 on.  */
    // holding place for this stuff.. does not work obvs needs to be in a loop
    while(1) {
        if ((PIND & (1<<7)) != 0) {
            PORTC |= (1 << 2); // turn on LED
        } else {
            PORTC &= ~(1 << 2); // turn off LED
        }
    }
}

void disp_icons(void)
{
    uint8_t state = 0;
    uint8_t num_states = 3;
    char states[3] = {'R', 'P', 'S'}; 
    uint8_t switchDirection = 0;


    while(1) {
        pacer_wait();
        navswitch_update ();

        if (navswitch_push_event_p(NAVSWITCH_EAST) || switchDirection == FROM_LOOP_EAST) {
            init_matrix();
            state = (state + 1) % num_states;
            switchDirection = 0;
        } else if (navswitch_push_event_p(NAVSWITCH_WEST) || switchDirection == FROM_LOOP_WEST) {
            init_matrix();
            state = (state - 1 + num_states) % num_states;
            switchDirection = 0;
        }

        if (states[abs(state)] == 'P') {
            switchDirection = disp_paper();
        } else if (states[abs(state)] == 'R') {
            switchDirection = disp_rock();
        } else if (states[abs(state)] == 'S') {
            switchDirection = disp_scissors();
        }

    }
}


void init_all(void)
{
    system_init ();
    init_matrix();
    pacer_init(PACER_RATE);
    init_blue_led();
    navswitch_init ();
    init_text();
}

/*Displays welcome message and tutorial*/
void start_game(void)
{
    disp_welcome();
    dispTutorial();
    char options[] = {'Y', 'N'};
    char character = selectVal(options, 2);

}


int main (void)
{
    init_all();
    start_game();


}
