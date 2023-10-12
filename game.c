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

#define PSR_COUNTDOWN_TIME 750

#define MAXIMUM_ASCII_VALUE 265
#define MINIMUM_ASCII_VALUE 0
#define NUMBER_OF_ICONS 3

void show_display(void(*displayfunc)(void));
void displayTutorial(void);
void scrolling_text(char* text);
char selectAndDisplayOptions(char* states, uint8_t n);
void game_welcome(void);
void send_char(char character);
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
    our_timer_init();
}


void show_display(void(*displayfunc)(void))
{
    while(1) {
        pacer_wait();
        displayfunc();
        navswitch_update();
        if (direction_moved() != 0) {
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
        displayfunc();
    }
    matrix_init();

}

void displayTutorial(void)
{
    scrolling_text("Rock\0");
    show_display(&display_north_arrow);
    show_display(&display_rock);

    scrolling_text("Paper\0");
    show_display(&display_east_arrow);
    show_display(&display_paper);

    scrolling_text("Scissors\0");
    show_display(&display_west_arrow);
    show_display(&display_scissors);
    
}

void scrolling_text(char* text)
{
    init_text(text);
    show_display(&disp_text);

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

/*Displays welcome message and tutorial*/
void game_welcome(void)
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


char receive_char(char upperBound, char lowerBound)
{
    char character = '0';
    
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
    char numRounds = '0';

    // need to figure out a way to display first then capture
    // ######### currently you need to push down to access the rounds!!!
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
            disp_character(numRounds);
        }
    }

    return numRounds;
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
