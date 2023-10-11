#include <avr/io.h>
#include <stdlib.h>
#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "led.h"
#include "navswitch.h"
#include "matrix.h"
#include "nav.h"

#define PACER_RATE  500/* Pacer loop - controls main loop */
#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define NUMBER_OF_CHOICES_FOR_START 2
#define NUMBER_OF_CHOICES_FOR_ROUNDS 5


/*
void init_all(void)
{
    system_init ();
    matrix_init();
    pacer_init(PACER_RATE);
    navswitch_init ();
    init_text();
}

*/


/*
void displayTutorial(void)
{
    disp_text("Rock\0");
    display_rock();
    //concurrent checking for moving switch - need general move switch function
    display_up_arrow();
    // concurrent checking for moving switch(NORTH) then when you have it will move on.
    display_text("Paper\0");
    display_right_arrow();
    display_paper();
    display_text("Scissors\0");
    display_left_arrow();
    display_scissors();

}

*/






// JR note move this to user input .c??
char selectAndDisplayOptions(char* states, uint8_t n)
{
    uint8_t state = 0;
    matrix_init();
    while (1) {
        pacer_wait();
        navswitch_update();
        disp_text();

        if (is_goal_nav(EAST)) {
            state = (state + 1) % n;
        } else if (is_goal_nav(WEST)) {
            state = (state - 1 + n) % n;
        }

        disp_character(states[state]);

        if (is_goal_nav(PUSH)) {
            matrix_init();
            return states[state];
        }
    }

}

void scrolling_text(char* text)
{
    init_text(text);

    while (1) {
        pacer_wait();
        disp_text();
        navswitch_update();
        if (direction_moved() != 0) {
            matrix_init();
            break;
        }
    }
}


void welcome_game(void)
{
    scrolling_text("Welcome to PSR! Move to start\0");
    scrolling_text("View tutorial?\0");

    char options[] = {'Y', 'N'};

    char character = selectAndDisplayOptions(options, 2);

    //if (character == 'Y') {
        //displayTutorial();
    //}

}


int main (void)
{
    pacer_init(PACER_RATE);

    welcome_game();

    //start_game();
    //setup_game();

}
