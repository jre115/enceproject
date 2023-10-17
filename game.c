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

typedef enum {
    INDIVIDUAL,
    DUAL
} displayMode_t;


/* Define the global variables */ // TODO: make these not global maybe
static char prevDir = NO_DIRECTION;
static char other = NO_DIRECTION;
static int8_t playerScore = 0;


/* Define functions so not to have to worry about order */
char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode);
void disp_overall_result(char player);
char set_num_rounds(void);
void game_start(char roundsChar, char player);
char game_welcome(void);
int8_t game_result(void);


void init_all(void)
{
    system_init ();
    matrix_init();
    navswitch_init ();
    ir_uart_init();
    pacer_init(PACER_RATE);
}


// should this go in the display modual?
char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode)
{
    char character;
    uint8_t state = 0;
    uint16_t tick = 0;

    matrix_init();
    navswitch_update();

    while (1) {
        pacer_wait();

        disp_character(states[state]);
        matrix_disp_text();

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


char set_num_rounds(void)
{
    scrolling_text("How many rounds?\0");
    char roundOptions[NUMBER_OF_CHOICES_FOR_ROUNDS] = {'1', '3', '5', '7', '9'};

    char character = selectAndDisplayOptions(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS, DUAL);
    matrix_init();
    
    char result[10] = "Chosen 0 \0";
    result[7] = character;
    scrolling_text(result);

    return character;
}


void disp_overall_result(char player)
{
    char playerScoreAsChar = playerScore + '0';
    char otherScore = send_receive(player, playerScoreAsChar);

    if (playerScoreAsChar > otherScore) {
        char result[11] = "Winner: 0\0";
        result[8] = playerScoreAsChar;
        scrolling_text(result);
    } else if (playerScoreAsChar < otherScore) {
        char result[10] = "Loser: 0\0";
        result[7] = playerScoreAsChar;
        scrolling_text(result);
    } else {
        char result[8] = "Draw: 0\0";
        result[6] = playerScoreAsChar;
        scrolling_text(result);
    }
}    


void game_start(char roundsChar, char player)
{
    uint8_t rounds = roundsChar - '0';
    scrolling_text("Ready?\0");
    wait(player);
    uint8_t round = 0;
    while (round < rounds) {
        led_set(LED1, 0);
        // play a game of paper sissors rock and display winner
        other = NO_DIRECTION;
        prevDir = NO_DIRECTION;
        icon_countdown(&prevDir, &other);
        display_own(&prevDir, &other);
        int8_t result = game_result();
        display_game_result(result, &prevDir, &other);
        round++;
    }
}


/*Displays welcome message and tutorial*/
char game_welcome(void)
{
    led_set(LED1, 1);
    char player = player1_player2();
    // TODO: when should we remove??
    if (player == PLAYER2) {
        led_set(LED1, 0); // Blue LED on
    }

    scrolling_text("Welcome to PSR! Move to start\0");
    scrolling_text("View tutorial?\0");

    char options[NUMBER_OF_CHOICES_FOR_START] = {'Y', 'N'};

    char character = selectAndDisplayOptions(options, NUMBER_OF_CHOICES_FOR_START, INDIVIDUAL);

    if (character == 'Y') {
        displayTutorial();
    }

    wait(player);

    return player;
}


int8_t game_result(void) 
{
    int8_t result = 0;
    if (other == prevDir) {
        result = 0;
    } else if (other == NO_DIRECTION) {
        result = 1;
        playerScore++;
    } else if (prevDir == NO_DIRECTION) {
        result = -1;
    } else if (prevDir == ROCK && other == SCISSORS) {
        result = 1;
        playerScore++;
    } else if (prevDir == PAPER && other == ROCK) {
        result = 1;
        playerScore++;
    } else if (prevDir == SCISSORS && other == PAPER) {
        result = 1;
        playerScore++;
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
        game_start(numRounds, player);
        
        disp_overall_result(player);
        
        scrolling_text("Play Again?\0");

        char options[] = {'Y', 'N'};

        wait(player);
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