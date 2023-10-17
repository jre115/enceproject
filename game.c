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

#define PACER_RATE  500 /* Pacer loop - controls main loop */
#define PRESCALAR 1024
#define CPU_F 8000000
#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define NUMBER_OF_CHOICES_FOR_START 2
#define NUMBER_OF_CHOICES_FOR_ROUNDS 5
#define TRUE 1
#define FALSE 0

#define NULLCHAR '\0'

#define PSR_COUNTDOWN_TIME 750
#define YOUR_CHOICE_TIME 1500
#define RESULT_DISPLAY_TIME 3000

#define MAXIMUM_ASCII_VALUE 265
#define MINIMUM_ASCII_VALUE 0
#define NUMBER_OF_ICONS 3

#define RECEVPLAYER2 '8'
#define RECEVPLAYER1 '6'

#define ROCK NORTH
#define PAPER EAST
#define SCISSORS WEST

#define PLAYER1 'A'
#define PLAYER2 'E'


static char prevDir = NO_DIRECTION;
static char other = NO_DIRECTION;
static int8_t playerScore = 0;

void show_display(void(*displayfunc)(void), char direction);
void displayTutorial(void);
void scrolling_text(char* text);
//char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode);
char game_welcome(void);
char send_char(char character);
char receive_char(char upperBound, char lowerBound);
char set_num_rounds(void);
void rotate_through_icons(void);
void game_start(char roundsChar, char player);

void init_all(void)
{
    system_init ();
    matrix_init();
    navswitch_init ();
    ir_uart_init();
}


void show_display(void(*displayfunc)(void), char direction)
{
    while(1) {
        pacer_wait();
        displayfunc();
        navswitch_update();
        if (((direction_moved() != NO_DIRECTION) && direction == ANY) || (is_goal_nav(direction))){
            matrix_init();
            break;
        }
    }
}


void displayTutorial(void)
{
    scrolling_text("Rock\0");
    show_display(&display_north_arrow, NORTH);
    show_display(&display_rock, NORTH);

    scrolling_text("Paper\0");
    show_display(&display_east_arrow, EAST);
    show_display(&display_paper, EAST);

    scrolling_text("Scissors\0");
    show_display(&display_west_arrow, WEST);
    show_display(&display_scissors, WEST);
}

void scrolling_text(char* text)
{
    init_text(text);
    show_display(&disp_text, ANY);

}

typedef enum {
    INDIVIDUAL,
    DUAL
} displayMode_t;

// JR note move this to user input .c??

char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode)
{
    // add uart in here
    uint8_t state = 0;
    matrix_init();
    uint16_t tick = 0;
    char character;
    navswitch_update();
    ir_uart_init();

    uint16_t sendRate = 20; // #define??

    
    while (1) {
        pacer_wait();

        disp_character(states[state]);
        disp_text();


        if (mode == DUAL && ir_uart_read_ready_p()) {
            character = ir_uart_getc();
            if (character != 'P') {
                state = character - '0';
            } else {
                return states[state];
            }
            
        } else if (tick > PACER_RATE / sendRate) {
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


void wait(char player) {

    uint16_t tick = 0;
    char character;
    uint16_t sendRate = 1;
    navswitch_init();
    uint8_t recev = 0;



    if (player == PLAYER1) {
        while(recev == 0) {
            tick += 1;
            display_sand_timer();
            pacer_wait();
            if (tick > PACER_RATE / sendRate) {
                tick = 0;
                ir_uart_putc(player);
            }
    
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER2) {
                    recev = 1;
                }
            }
        }
    } else if (player == PLAYER2) {
        while (recev == 0) {
            display_sand_timer();
            pacer_wait();
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER1) {
                    recev = 1;
                }
            }
        }

        ir_uart_putc(player);
        pacer_wait();
        ir_uart_putc(player);
        pacer_wait();
        ir_uart_putc(player);

    }
}

char player1_player2(void)
{
    int current = 0;
    int decided = 0; 
    char player = 'A';
    char sendChar = 'B';

    while (!decided) {
        PORTC |= (1 << 2); /// remove this

        navswitch_update();
        
        if (direction_moved() != NO_DIRECTION) {
            decided = 1;
            ir_uart_putc(sendChar);
            current = PLAYER1;
        }

        if (ir_uart_read_ready_p()) {
            player = ir_uart_getc();
        } if (player == 'B') {
            decided = 1;
            current = PLAYER2;
        }
    }

    PORTC &= ~(1 << 2); // led off
    return current;
}


/*Displays welcome message and tutorial*/
char game_welcome(void)
{
    char player = player1_player2();
    if (player == PLAYER1) {
        PORTC |= (1 << 2);
    }


    scrolling_text("Welcome to PSR! Move to start\0");
    scrolling_text("View tutorial?\0");

    char options[] = {'Y', 'N'};

    char character = selectAndDisplayOptions(options, 2, INDIVIDUAL);

    if (character == 'Y') {
        displayTutorial();
    }

    wait(player);

    return player;
}


char set_num_rounds(void)
{
    scrolling_text("How many rounds?\0");
    char roundOptions[] = {'1', '3', '5', '7', '9'};
    uint8_t numOptions = 5;

    char character = selectAndDisplayOptions(roundOptions, numOptions, DUAL);
    matrix_init();
    
    char result[10] = "Chosen 0 \0";
    result[7] = character;
    scrolling_text(result);

    return character;
}


void timed_display(void(*displayfunc)(void), uint16_t milliseconds)
{
    uint16_t ticks = (milliseconds) * (CPU_F / PRESCALAR) / 1000;
    char direction;
    matrix_init();

    TCNT1 = 0;
    while (TCNT1 < ticks) {
        navswitch_update();
        // push event:
        // do this
        if (ir_uart_read_ready_p()) {
            other = ir_uart_getc();
        }
        direction = direction_moved();
        if (direction == NORTH || direction == EAST or direction == WEST) {
            prevDir = direction;
            ir_uart_putc(prevDir);
            PORTC |= (1 << 2); // led on
        }
        displayfunc();
    }
    matrix_init();
}

void icon_countdown(void) 
{
    
    timed_display(&display_paper, PSR_COUNTDOWN_TIME);
    //ir_uart_putc(prevDir);
    timed_display(&display_scissors, PSR_COUNTDOWN_TIME);
    //ir_uart_putc(prevDir);
    timed_display(&display_rock, PSR_COUNTDOWN_TIME);
    //ir_uart_putc(prevDir);
    timed_display(&display_none, PSR_COUNTDOWN_TIME / 2);

}

void display_own(void) {
    pacer_wait();
    if (prevDir == ROCK) {
        timed_display(&display_rock, YOUR_CHOICE_TIME);
    } else if (prevDir == PAPER) {
        timed_display(&display_paper, YOUR_CHOICE_TIME);
    } else if (prevDir == SCISSORS) {
        timed_display(&display_scissors, YOUR_CHOICE_TIME);
    } else {
        timed_display(&display_none, YOUR_CHOICE_TIME);
    }
}
int8_t game_result(void) {
    int8_t result = 0;
    if (other == prevDir) {
        result = 0;
    } else if (other == NO_DIRECTION) {
        result = 1;
    } else if (prevDir == NO_DIRECTION) {
        result = -1;
        playerScore = playerScore + 1;
    } else if (other == ROCK) {
        if (prevDir == SCISSORS) {
            result = -1;
        } else if (prevDir == PAPER) {
            result = 1;
        }
    } else if (other == PAPER) {
        if (prevDir == ROCK) {
            result = -1;
        } else if (prevDir == SCISSORS) {
            result = 1;
        }

    } else if (other == SCISSORS) {
        if (prevDir == PAPER) {
            result = -1;
        } else if (prevDir == ROCK) {
            result = 1;
        }
    }

    return result;
}

void display_game_result(int8_t result) 
{
    if (result == -1) {
        timed_display(&display_sad_face, RESULT_DISPLAY_TIME);
    } else if (result == 0) {
        timed_display(&display_draw_face, RESULT_DISPLAY_TIME);
    } else if (result == 1) {
        timed_display(&display_smiley_face, RESULT_DISPLAY_TIME);
    }
}



void game_start(char roundsChar, char player)
{
    uint8_t rounds = roundsChar - '0';
    // PORTC &= ~(1 << 2); // led off
    scrolling_text("Ready?\0");
    wait(player);
    uint8_t round = 0;
    while (round < rounds) {
        // play a game of paper sissors rock and display winner
        PORTC &= ~(1 << 2);
        other = NO_DIRECTION;
        prevDir = NO_DIRECTION;
        icon_countdown();
        display_own();
        int8_t result = game_result();

        // if there is a tie, you redo it ! so add another roundNO_DIRECTION
        if (result == 0) {
            rounds += 1;
        } else {
            display_game_result(result);
        }
        round++;

    }
}

char send_receive(char player, char message)
{
    char receivedMessage = NO_DIRECTION;
NO_DIRECTION
    if (player == PLAYER1) {
        ir_uart_putc(message);
        pacer_wait();
        receivedMessage = ir_uart_getc();
    } else if (player == PLAYER2) {
        receivedMessage = ir_uart_getc();
        pacer_wait();
        ir_uart_putc(message);
    }

    return receivedMessage;
    
}



/*
char send_receive(char player, char message)
{   
    char received_message;
    int16_t ticks = 0;
    char tempPlayer = player;
    
    while (1) {
        pacer_wait();
        if (tempPlayer == PLAYER1) {
            // Player 1 sends messages at 500 Hz for 1 second
            if (ticks < 500) {
                ir_uart_putc(message);
                pacer_wait();received_message
            } else {
                tempPlayer = PLAYER2;  // Switch roles
            }
        } else if (tempPlayer == PLAYER2) {
            if (ir_uart_read_ready_p()) {
                received_message = ir_uart_getc();
                if (player == PLAYER1) {
                    break;
                } else {
                    tempPlayer = PLAYER1;  // Switch roles
                }
                
            }

            }
            
        }
        return received_message;

}
*/

void disp_overall_result(char player)
{
    char playerScoreAsChar = playerScore + '0';
    char otherScore = send_receive(player, playerScoreAsChar);

    if (playerScoreAsChar > otherScore) {
        char result[12] = "Winner: 0 \0";
        result[8] = playerScoreAsChar;
        scrolling_text(result);
    } else {
        char result[12] = "Loser: 0 \0";
        result[7] = playerScoreAsChar;
        scrolling_text(result);
    }
}




int main (void)
{
    init_all();
    pacer_init(PACER_RATE);
    int8_t playAgain;
    char player = game_welcome(); // whoop whoop this is all good :)
    
    do {
        char numRounds = set_num_rounds();
        game_start(numRounds, player);
        
        disp_overall_result(player);
        
        scrolling_text("Play Again?\0");

        char options[] = {'Y', 'N'};

        char character = selectAndDisplayOptions(options, 2, INDIVIDUAL);

        if (character == 'Y') {
            playAgain = 1;
        } else {
            playAgain = 0;
        }

        wait(player);

    } while (playAgain);


    //start_game();
    //setup_game();

}
