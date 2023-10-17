/*
# File:   game.c
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  contains game logic for rock-paper-scissors game
*/

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
#include "tinygl.h"
#include "communication.h"
#include "displays.h"

#define PACER_RATE 500
#define NUMBER_OF_CHOICES_FOR_START 2
#define NUMBER_OF_CHOICES_FOR_ROUNDS 5
#define SEND_RATE_DISPLAY 20

// these should be in the other header files!!
#define PLAYER1 'A'
#define PLAYER2 'E'
#define ROCK NORTH
#define PAPER EAST
#define SCISSORS WEST

/* used to represent the different type of display modes. Dual if the toggles
and push to select work across both fun kits. Otherwise individual.*/
typedef enum {
    INDIVIDUAL,
    DUAL
} displayMode_t;


/* Define the global variables */ // TODO: make these not global maybe
static char prevDir = NO_DIRECTION;
static char other = NO_DIRECTION;


/* Define functions so not to have to worry about order */
char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode);
void determine_and_displays_overall_result(char player, int8_t playerScore);
char set_num_rounds(void);
int8_t game_start(char roundsChar, char player);
char game_welcome(void);
int8_t game_result(int8_t* playerScorePtr);

/* initialises all required */
void init_all(void)
{
    system_init ();
    matrix_init();
    navswitch_init ();
    ir_uart_init();
    pacer_init(PACER_RATE);
}


/* Select and display options on the matrix display. 
Users navigate through set of options using navswitch and can select an option. 
In DUAL mode it communicates with the other microcontroller */
char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode)
{
    char character;
    uint8_t state = 0;
    uint16_t tick = 0;

    matrix_init();
    navswitch_update();

    while (1) {
        pacer_wait();

        matrix_display_character(states[state]);
        matrix_disp_text();

        // check for incoming characters in dual mode.
        // If receives message that nav has been pushed: return current displayed
        if (mode == DUAL && ir_uart_read_ready_p()) {
            character = ir_uart_getc();
            if (character != 'P') {
                state = character - '0';
            } else {
                return states[state];
            }
        } else if (tick > PACER_RATE / SEND_RATE_DISPLAY) {
            navswitch_update();
            tick = 0;

            // Checks if the navswitch has been moved east or west and sends this to other microcontroller if in DUAL mode
            if (is_goal_nav(EAST)) {
                state = (state + 1) % n;
                if (mode == DUAL) {
                    ir_uart_putc(state + '0');
                }
            } else if (is_goal_nav(WEST)) {
                state = (state - 1 + n) % n;
                if (mode == DUAL) {
                    ir_uart_putc(state + '0');
                }
            // Sends message if navswitch has been pushed if in DUAL mode and returns current displayed
            } else if (is_goal_nav(PUSH)) {
                matrix_init();

                if (mode == DUAL) {
                    ir_uart_putc('P');
                }

                return states[state];
            }
        }
        tick++;
    }
}


/* Prompts the users to select the number of rounds they want to play and returns the number of rounds.
Either users can toggle and select the number of rounds*/
char set_num_rounds(void)
{
    displays_scrolling_text("How many rounds?\0");
    char roundOptions[NUMBER_OF_CHOICES_FOR_ROUNDS] = {'1', '3', '5', '7', '9'};

    char roundOptions[NUMBER_OF_CHOICES_FOR_ROUNDS] = {'1', '3', '5', '7', '9'};
    char character = selectAndDisplayOptions(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS, DUAL);
    
    // displays chosen number of rounds
    matrix_init();
    char result[10] = "Chosen 0 \0";
    result[7] = character;
    displays_scrolling_text(result);

    return character;
}

/* Prompts the users to communicate their results and displays the compared results*/
void determine_and_displays_overall_result(char player, int8_t playerScore)
{   
    // converts the player score to a char for comparison
    char playerScoreAsChar = playerScore + '0';
    char otherScore = communication_send_and_recieve(player, playerScoreAsChar);

    displays_overall_result(playerScoreAsChar, otherScore);
}    


int8_t game_start(char roundsChar, char player)
{
    uint8_t rounds = roundsChar - '0';
    displays_scrolling_text("Ready?\0");
    communication_wait_for_other_player(player);
    uint8_t round = 0;
    int8_t playerScore = 0;
    while (round < rounds) {
        led_set(LED1, 0);
        // play a game of paper sissors rock and display winner
        other = NO_DIRECTION;
        prevDir = NO_DIRECTION;
        displays_icon_countdown(&prevDir, &other);
        displays_own(&prevDir, &other);
        int8_t result = game_result(&playerScore);
        displays_game_result(result, &prevDir, &other);
        round++;
    }
    return playerScore;
}


/*Displays welcome message and tutorial*/
char game_welcome(void)
{
    led_set(LED1, 1);
    char player = communication_player_setup();
    // TODO: when should we remove??
    if (player == PLAYER2) {
        led_set(LED1, 0); // Blue LED on
    }

    displays_scrolling_text("Welcome to PSR! Move to start\0");
    displays_scrolling_text("View tutorial?\0");

    char options[NUMBER_OF_CHOICES_FOR_START] = {'Y', 'N'};

    char character = selectAndDisplayOptions(options, NUMBER_OF_CHOICES_FOR_START, INDIVIDUAL);

    if (character == 'Y') {
        displays_tutorial();
    }

    communication_wait_for_other_player(player);

    return player;
}


int8_t game_result(int8_t* playerScorePtr) 
{
    int8_t result = 0;
    if (other == prevDir) {
        result = 0;
    } else if (other == NO_DIRECTION) {
        result = 1;
        (*playerScorePtr)++;
    } else if (prevDir == NO_DIRECTION) {
        result = -1;
    } else if (prevDir == ROCK && other == SCISSORS) {
        result = 1;
        (*playerScorePtr)++;
    } else if (prevDir == PAPER && other == ROCK) {
        result = 1;
        (*playerScorePtr)++;
    } else if (prevDir == SCISSORS && other == PAPER) {
        result = 1;
        (*playerScorePtr)++;
    } else {
        result = -1;
    }

    return result;
}


int main (void)
{
    init_all();

    int8_t playAgain;
    char player = game_welcome(); // whoop whoop this is all good :)
    
    do {
        char numRounds = set_num_rounds();
        int8_t playerScore = game_start(numRounds, player);
        determine_and_displays_overall_result(player, playerScore);
        
        displays_scrolling_text("Play Again?\0");

        char options[] = {'Y', 'N'};

        communication_wait_for_other_player(player);
        char character = selectAndDisplayOptions(options, 2, DUAL);

        if (character == 'Y') {
            playAgain = 1;
        } else {
            playAgain = 0;
            led_set(LED1, 0);
            matrix_init();
        }


    } while (playAgain);

}