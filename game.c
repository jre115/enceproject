#include <avr/io.h>
#include <stdlib.h>
#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "led.h"
#include "navswitch.h"
#include "matrix.h"
#include "nav.h"
#include "ir_uart.h"

#define PACER_RATE  500 /* Pacer loop - controls main loop */
#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define NUMBER_OF_CHOICES_FOR_START 2
#define NUMBER_OF_CHOICES_FOR_ROUNDS 5

#define MAXIMUM_ASCII_VALUE 265
#define MINIMUM_ASCII_VALUE 0
#define NUMBER_OF_ICONS 3
/*
void init_all(void)
{
    system_init ();
    matrix_init();
    pacer_init(PACER_RATE);
    navswitch_init ();
    init_text();
    ir_uart_init();
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



/*Displays welcome message and tutorial*/
void game_Welcome(void)
{
    scrolling_text("Welcome to PSR! Move to start\0");
    scrolling_text("View tutorial?\0");

    char options[] = {'Y', 'N'};

    char character = selectAndDisplayOptions(options, 2);

    if (character == 'Y') {
        displayTutorial();
    }

}

void send_char(char character)
{
    /* TODO: Transmit the character over IR on a NAVSWITCH_PUSH event.  */
    if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        ir_uart_putc(character);
    }
}


char receive_char(char upperBound, char lowerBound){
    char character = '0';
    
    if (ir_uart_read_ready_p()) {
        char temp_char = ir_uart_getc();
        if (temp_char >= lowerBound && temp_char <= upperBound) {
            character = temp_char;
        }
    }
    return character;
}

char set_num_rounds(void){
    disp_text("How many rounds?\0");
    char roundOptions[] = {'1', '3', '5', '7', '9'};
    char numRounds = '0';
    // display the choices
    // bool roundsSet = false;
    numRounds = selectVal(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS);
    // while (roundsSet == false) {
    //     if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
    //         // set to user of this boards value
    //         numRounds = selectVal(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS);
    //         roundsSet = true;
    //     } else if (ir_uart_read_ready_p()) {
    //         numRounds = receiveChar('1', '9');
    //         // got to check if it's 2, 3, 6 or 8
    //         // set to this value
    //         displayReceivedChar(numRounds);
    //     }
    // }
    return numRounds;
}

// char setup_game(void)
// {
//     return setNumRounds(); // getting the rounds from either this player or the other one

// }

void rotate_through_icons(void) 
{
    int16_t button_tick = 0;
    while(1) {
        pacer_wait();
        button_tick++;
        if ((button_tick - 60) > 0) {
            matrix_init();
            break;
        } else if ((button_tick - 45) > 0) {
            tinygl_text("Shoot\0");
        } else if ((button_tick - 30) > 0) {
            disp_rock();
        } else if ((button_tick - 15) > 0) {
            disp_scissors();
        } else {
            disp_paper();
        }
    }
    
}

void game_start(char roundsChar)
{
    uint8_t rounds = roundsChar - '0';
    for (uint8_t i = 0; i < rounds; i++) {
        // play a game of paper sissors rock and display winner
        rotateThroughIcons();
    }
}




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





int main (void)
{
    init_all();
    pacer_init(PACER_RATE);

    game_welcome();
    
    char numRounds = set_num_rounds();
    game_start(numRounds);

    //start_game();
    //setup_game();

}
