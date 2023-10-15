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

void show_display(void(*displayfunc)(void), uint8_t direction);
void displayTutorial(void);
void scrolling_text(char* text);
//char selectAndDisplayOptions(char* states, uint8_t n, displayMode_t mode);
void game_welcome(void);
char send_char(char character);
char receive_char(char upperBound, char lowerBound);
char set_num_rounds(void);
void rotate_through_icons(void);
void game_start(char roundsChar);

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

void timed_display(void(*displayfunc)(void), uint16_t milliseconds)
{
    uint16_t ticks = (milliseconds) * (CPU_F / PRESCALAR) / 1000;

    TCNT1 = 0;
    while (TCNT1 < ticks) {
        // push event:
        // do this
        displayfunc();
    }
    matrix_init();

    // overarching loop
    // run ticks
    // can just turn LED on, then turn off when doing IR stuff
    // make it fast enough that it looks like the display is always on,
    // inbetween this time though check for navswitch flicks which will set the users option
    // will need to change how this is done so that it works in the big loop.

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

    uint16_t sendRate = 20;


    
    while (1) {
        pacer_wait();

        disp_character(states[state]);
        disp_text();


        if (mode == DUAL && ir_uart_read_ready_p()) {
            character = ir_uart_getc();
            if (character != 'P') {
                state = (int)character;
            } else {
                return states[state];
            }
            
        } else if (tick > PACER_RATE / sendRate) {
            navswitch_update();

            tick = 0;
            if (is_goal_nav(EAST)) {
                state = (state + 1) % n;
                if (mode == DUAL) {
                    ir_uart_putc((char)state);
                }
            } else if (is_goal_nav(WEST)) {
                state = (state - 1 + n) % n;
                if (mode == DUAL) {
                    ir_uart_putc((char)state);
                }
            } else if (is_goal_nav(PUSH)) {
                matrix_init();
                if (mode == DUAL) {
                    ir_uart_putc('P');
                }
                return states[state];
            }
            
        }
        tick += 1;
    
    }

}

int wait_both_ready(void) {
    init_text("Waiting for other player...\0");
    uint16_t tick = 0;
    uint16_t totalCount = 0;
    char character;
    uint16_t timeout = 7500;
    uint16_t sendRate = 50;

    while (totalCount < timeout) {
        tick += 1;
        totalCount += 1;
        pacer_wait();
        disp_text();

        if (ir_uart_read_ready_p()) {
            character = ir_uart_getc();
            if (character == RECEVPLAYER2) {
                ir_uart_putc(RECEVPLAYER1);
                return 2;
            } else {
                return 1;
            }
        } else if (tick > PACER_RATE / sendRate) {
            tick = 0;
            ir_uart_putc(RECEVPLAYER2);
        }
    }

    if (direction_moved != 0) {
        return;
    }
}


/*Displays welcome message and tutorial*/
void game_welcome(void)
{
    scrolling_text("Welcome to PSR! Move to start\0");
    scrolling_text("View tutorial?\0");

    char options[] = {'Y', 'N'};

    char character = selectAndDisplayOptions(options, 2, INDIVIDUAL);

    if (character == 'Y') {
        displayTutorial();
    }

    wait_both_ready();


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
    
    char result[11] = "Chosen 0 \0";
    result[7] = character;
    scrolling_text(result);


    // need to figure out a way to display first then capture
    // ######### currently you need to push down to access the rounds!!!

    /*
    while (1) {
        pacer_wait();
        navswitch_update();
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            // set to user of this boards value
            numRounds = selectAndDisplayOptions(roundOptions, NUMBER_OF_CHOICES_FOR_ROUNDS);
            break;
        } else if (ir_uart_read_ready_p()) {
            numRounds = receive_char('1', '9');
            // got to check if it's 2, 3, 6 or 8
            // set to this value
            //break;
        }
        

    }

    
    while(1) {
        pacer_wait();
        disp_character(numRounds);
        navswitch_update();
        if (direction_moved() != 0) {
            matrix_init();
            break;
        }
    }

    return numRounds;
    */
}


void icon_countdown(void) 
{
    timed_display(&display_paper, PSR_COUNTDOWN_TIME);
    timed_display(&display_scissors, PSR_COUNTDOWN_TIME);
    timed_display(&display_rock, PSR_COUNTDOWN_TIME);
}

void game_start(char roundsChar)
{
    uint8_t rounds = roundsChar - '0';
    for (uint8_t i = 0; i < rounds; i++) {
        // play a game of paper sissors rock and display winner
        scrolling_text("Ready?\0");
        icon_countdown();
        
    }
}



int main (void)
{
    init_all();
    pacer_init(PACER_RATE);

    game_welcome(); // whoop whoop this is all good :)
    
    char numRounds = set_num_rounds();
    game_start(numRounds);

    //start_game();
    //setup_game();

}
