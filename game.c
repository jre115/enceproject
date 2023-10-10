#include <avr/io.h>
#include <stdlib.h>
#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "led.h"
#include "navswitch.h"
#include "matrix.h"

#define PACER_RATE  500/* Pacer loop - controls main loop */
#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define NUMBER_OF_CHOICES_FOR_START 2
#define NUMBER_OF_CHOICES_FOR_ROUNDS 5

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
            matrix_init();
            state = (state + 1) % num_states;
            switchDirection = 0;
        } else if (navswitch_push_event_p(NAVSWITCH_WEST) || switchDirection == FROM_LOOP_WEST) {
            matrix_init();
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
    matrix_init();
    pacer_init(PACER_RATE);
    navswitch_init ();
    init_text();
}

/*Displays welcome message and tutorial*/
void start_game(void)
{
    disp_text("Welcome to PSR! Move to start\0");
    disp_text("View tutorial?\0");
    char options[] = {'Y', 'N'};
    char character = selectVal(options, NUMBER_OF_CHOICES_FOR_START);

    if (character == 'Y') {
        displayTutorial();
    }

}

void setup_game(void)
{
    disp_text("How many rounds?\0");
    char roundOptions[] = {'1', '3', '5', '7', '9'};
    // char roundNum = selectVal(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS);
    selectVal(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS);
}


int main (void)
{
    init_all();
    start_game();
    setup_game();

}
