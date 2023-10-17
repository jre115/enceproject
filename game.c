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
#define SEND_RATE_DISPLAY 20
#define NUMBER_OF_CHOICES_FOR_START 2
#define NUMBER_OF_CHOICES_FOR_ROUNDS 5

#define PLAYER1 'A'
#define PLAYER2 'E'

#define ROCK NORTH
#define PAPER EAST
#define SCISSORS WEST

#define LOSS -1
#define DRAW 0
#define WIN 1


/* used to represent the different type of display modes. Dual if the toggles
and push to select work across both fun kits. Otherwise individual.*/
typedef enum {
    INDIVIDUAL,
    DUAL
} displayMode_t;


/* Define functions so not to have to worry about order */
char game_select_and_display_options(char* states, uint8_t n, displayMode_t mode);
void game_determine_and_display_overall_result(char player, int8_t player_score);
char game_set_num_rounds(void);
int8_t game_start(char rounds_char, char player);
char game_welcome(void);
int8_t game_result(int8_t* player_score_ptr, char* previous_direction, char* other_players_direction);


/* initialises all required */
void init_all(void)
{
    system_init();
    matrix_init();
    navswitch_init();
    ir_uart_init();
    pacer_init(PACER_RATE);
}


/* Select and display options on the matrix display. 
Users navigate through set of options using navswitch and can select an option. 
In DUAL mode it communicates with the other_players_direction microcontroller */
char game_select_and_display_options(char* states, uint8_t n, displayMode_t mode)
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

            // Checks if the navswitch has been moved east or west and sends this to other_players_direction microcontroller if in DUAL mode
            if (nav_is_goal(EAST)) {
                state = (state + 1) % n;
                if (mode == DUAL) {
                    ir_uart_putc(state + '0');
                }
            } else if (nav_is_goal(WEST)) {
                state = (state - 1 + n) % n;
                if (mode == DUAL) {
                    ir_uart_putc(state + '0');
                }
            // Sends message if navswitch has been pushed if in DUAL mode and returns current displayed
            } else if (nav_is_goal(PUSH)) {
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
char game_set_num_rounds(void)
{
    displays_scrolling_text("How many rounds?\0");
    char roundOptions[NUMBER_OF_CHOICES_FOR_ROUNDS] = {'1', '3', '5', '7', '9'};
    char character = game_select_and_display_options(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS, DUAL);
    
    // displays chosen number of rounds
    matrix_init();
    char result[10] = "Chosen 0 \0";
    result[7] = character;
    displays_scrolling_text(result);

    return character;
}


/* Prompts the users to communicate their results and displays the compared results*/
void game_determine_and_display_overall_result(char player, int8_t player_score)
{   
    // converts the player score to a char for comparison
    char playerScoreAsChar = player_score + '0';
    char otherScore = communication_send_and_recieve(player, playerScoreAsChar);

    displays_overall_result(playerScoreAsChar, otherScore);
}    


/*
 * Plays a round-based game of Paper, Scissors, Rock (PSR) for the specified number of rounds,
 * updating the player's score based on the outcomes of each round.
 */
int8_t game_start(char rounds_char, char player)
{
    uint8_t rounds = rounds_char - '0';
    displays_scrolling_text("Ready?\0");
    communication_wait_for_other_player(player);

    uint8_t round = 0;


    int8_t player_score = 0;
    while (round < rounds) {
        // reset light and selected moves at the start of each round
        led_set(LED1, 0); 
        char other_players_direction = NO_DIRECTION;
        char previous_direction = NO_DIRECTION;

        // play a game of paper sissors rock then displays their own selected move
        displays_icon_countdown(&previous_direction, &other_players_direction);
        displays_own(&previous_direction, &other_players_direction);

        // displays the result of the game
        int8_t result = game_result(&player_score, &previous_direction, &other_players_direction);
        displays_game_result(result, &previous_direction, &other_players_direction);

        round++;
    }

    return player_score;
}


/* Displays welcome message and tutorial */
char game_welcome(void)
{
    // Switches on LED to indicate on
    led_set(LED1, 1);

    char player = communication_player_setup();
    
    led_set(LED1, 0); 

    displays_scrolling_text("Welcome to PSR! Move to start\0");

    // select Y or N for tutorial
    displays_scrolling_text("View tutorial?\0");
    char options[NUMBER_OF_CHOICES_FOR_START] = {'Y', 'N'};
    char character = game_select_and_display_options(options, NUMBER_OF_CHOICES_FOR_START, INDIVIDUAL);
    if (character == 'Y') {
        displays_tutorial();
    }

    // waits to ensure players are in sync
    communication_wait_for_other_player(player);

    return player;
}


/* Determines the result of a round based on the player's and the opponent's chosen moves.
    Updates the player's score accordingly. */
int8_t game_result(int8_t* player_score_ptr, char* previous_direction, char* other_players_direction) 
{
    int8_t result = DRAW;
    if ((*other_players_direction) == (*previous_direction)) {
        result = DRAW;
    } else if ((*other_players_direction) == NO_DIRECTION) {
        result = WIN;
        (*player_score_ptr)++;
    } else if ((*previous_direction) == NO_DIRECTION) {
        result = LOSS;
    } else if ((*previous_direction) == ROCK && (*other_players_direction) == SCISSORS) {
        result = WIN;
        (*player_score_ptr)++;
    } else if ((*previous_direction) == PAPER && (*other_players_direction) == ROCK) {
        result = WIN;
        (*player_score_ptr)++;
    } else if ((*previous_direction) == SCISSORS && (*other_players_direction) == PAPER) {
        result = WIN;
        (*player_score_ptr)++;
    } else {
        // otherwise player loses
        result = LOSS; 
    }

    return result;
}


/* Main function to initialise the game, handle game flow,
and allow players to play multiple rounds. */
int main (void)
{
    init_all();
    int8_t playAgain = TRUE;

    char player = game_welcome();
    
    // Continues with round selection and playing the rounds until the user does not want to play again
    do {
        char numRounds = game_set_num_rounds();
        int8_t player_score = game_start(numRounds, player);
        game_determine_and_display_overall_result(player, player_score);
        
        displays_scrolling_text("Play Again?\0");

        // wait to be in sync
        communication_wait_for_other_player(player);

        char options[] = {'Y', 'N'};
        char character = game_select_and_display_options(options, 2, DUAL);

        // If play again is not selected (N = NO), exit the game, otherwise loop again
        if (character == 'N') {
            playAgain = FALSE;
            led_set(LED1, 0);
            matrix_init();
        }

    } while (playAgain);

}
