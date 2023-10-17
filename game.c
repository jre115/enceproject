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

#define MAXIMUM_ASCII_VALUE 265
#define MINIMUM_ASCII_VALUE 0
#define NUMBER_OF_ICONS 3

#define RECEVPLAYER2 '8'
#define RECEVPLAYER1 '6'

#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4
#define PUSH 5

#define OTHER_ROCK '0'
#define OTHER_PAPER '1'
#define OTHER_SCISSORS '2'
#define ROCK NORTH
#define PAPER EAST
#define SCISSORS WEST

#define PLAYER1 'A'
#define PLAYER2 'E'


static uint8_t prevDir = 0;
static char other;

void show_display(void(*displayfunc)(void), uint8_t direction);
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


void show_display(void(*displayfunc)(void), uint8_t direction)
{
    while(1) {
        pacer_wait();
        displayfunc();
        navswitch_update();
        if (((direction_moved() != 0) && direction == ANY) || (is_goal_nav(direction))){
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

    uint16_t sendRate = 20; // #define??


    
    while (1) {
        pacer_wait();

        disp_character(states[state]);
        disp_text();


        if (mode == DUAL && ir_uart_read_ready_p()) {
            character = ir_uart_getc();
            if (character != 'P') {
                state = (int)character; /// It must be an (int) to work..... WHY??
            } else {
                return states[state];
            }
            
        } else if (tick > PACER_RATE / sendRate) {
            navswitch_update();

            tick = 0;
            if (is_goal_nav(EAST)) {
                state = (state + 1) % n;
                if (mode == DUAL) {
                    ir_uart_putc(state);
                }
            } else if (is_goal_nav(WEST)) {
                state = (state - 1 + n) % n;
                if (mode == DUAL) {
                    ir_uart_putc(state);
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

     
    init_text("Waiting for other player...\0");
    uint16_t tick = 0;
    char character;
    uint16_t sendRate = 20;
    navswitch_init();
    uint8_t recev = 0;



    if (player == PLAYER1) {
        while(recev == 0) {
            tick += 1;
            disp_text();
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
            disp_text();
            pacer_wait();
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER1) {
                    recev = 1;
                }
            }
        }

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
        
        if (direction_moved() != 0) {
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

char send_char(char character)
{
    /* TODO: Transmit the character over IR on a NAVSWITCH_PUSH event.  */
    if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
        ir_uart_putc(character);
        return character;
    }

    return NULLCHAR;
}


char receive_char(char upperBound, char lowerBound)
{
    char character = NULLCHAR;
    
    if (ir_uart_read_ready_p()) {
        char temp_char = ir_uart_getc();
        if (temp_char >= lowerBound && temp_char <= upperBound) {
            character = temp_char;
        }
    }
    return character;
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
    uint8_t direction;
    matrix_init();


    // uint8_t prevDir = 0;

    TCNT1 = 0;
    while (TCNT1 < ticks) {
        navswitch_update();
        // push event:
        // do this
        if (ir_uart_read_ready_p()) {
            other = ir_uart_getc();
        }
        direction = direction_moved();
        if (direction != 0) {
            prevDir = direction;
            ir_uart_putc(prevDir);
            PORTC |= (1 << 2); // led on
        }
        displayfunc();
    }
    matrix_init();

    // return prevDir;

    // overarching loop
    // run ticks
    // can just turn LED on, then turn off when doing IR stuff
    // make it fast enough that it looks like the display is always on,
    // inbetween this time though check for navswitch flicks which will set the users option
    // will need to change how this is done so that it works in the big loop.
}

void icon_countdown(void) 
{
    
    timed_display(&display_paper, PSR_COUNTDOWN_TIME);
    ir_uart_putc(prevDir);
    timed_display(&display_scissors, PSR_COUNTDOWN_TIME);
    ir_uart_putc(prevDir);
    timed_display(&display_rock, PSR_COUNTDOWN_TIME);
    ir_uart_putc(prevDir);

}


void game_result(void) {

    pacer_wait();
    if (other == OTHER_ROCK) {
        timed_display(&display_rock, PSR_COUNTDOWN_TIME);
    } else if (other == OTHER_PAPER) {
        timed_display(&display_paper, PSR_COUNTDOWN_TIME);
    } else if (other == OTHER_SCISSORS) {
        timed_display(&display_scissors, PSR_COUNTDOWN_TIME);
    } else {
        timed_display(&display_none, PSR_COUNTDOWN_TIME);
    }

    pacer_wait();
    if (prevDir == NORTH) {
        timed_display(&display_rock, PSR_COUNTDOWN_TIME);
    } else if (prevDir == EAST) {
        timed_display(&display_paper, PSR_COUNTDOWN_TIME);
    } else if (prevDir == WEST) {
        timed_display(&display_scissors, PSR_COUNTDOWN_TIME);
    } else {
        timed_display(&display_none, PSR_COUNTDOWN_TIME);
    }










    /*
    char this_choice;
    if (prevDir == ROCK) {
        this_choice = OTHER_ROCK;
    } else if (prevDir == PAPER) {
        this_choice = OTHER_PAPER;
    } else if (prevDir == SCISSORS) {
        this_choice = OTHER_SCISSORS;
    } else {
        this_choice = 'N';
    }

    matrix_init();
    disp_character(prevDir);
    while(1) {
        disp_text();
    }
    

    if (this_choice == 'N') {
        scrolling_text("NOTHING\0");
    }

    if (other == this_choice) {
        if (prevDir == NORTH) {
            timed_display(&display_rock, PSR_COUNTDOWN_TIME);  // if rock draw
        } else if (prevDir == EAST) {
            timed_display(&display_paper, PSR_COUNTDOWN_TIME);  // if paper draw
        } else if (prevDir == WEST) {
            timed_display(&display_scissors, PSR_COUNTDOWN_TIME);  // if scissors draw
        }
        
    } else if (other == OTHER_ROCK) {
        if (prevDir == SCISSORS) {
            scrolling_text("LOSE\0");
        } else if (prevDir == PAPER) {
            scrolling_text("WIN\0");
        }

    } else if (other == OTHER_PAPER) {
        if (prevDir == ROCK) {
            scrolling_text("LOSE\0");
        } else if (prevDir == SCISSORS) {
            scrolling_text("WIN\0");
        }

    } else if (other == OTHER_SCISSORS) {
        if (prevDir == PAPER) {
            scrolling_text("LOSE\0");
        } else if (prevDir == ROCK) {
            scrolling_text("WIN\0");
        }
    } else {
        scrolling_text("NA\0");
    }

    */
}


void game_start(char roundsChar, char player)
{
    uint8_t rounds = roundsChar - '0';
    // PORTC &= ~(1 << 2); // led off
    for (uint8_t i = 0; i < rounds; i++) {
        // play a game of paper sissors rock and display winner
        scrolling_text("Ready?\0");
        wait(player);
        icon_countdown();
        game_result();
        
    }
}



int main (void)
{
    init_all();
    pacer_init(PACER_RATE);

    char player = game_welcome(); // whoop whoop this is all good :)
    
    char numRounds = set_num_rounds();
    game_start(numRounds, player);

    //start_game();
    //setup_game();

}
