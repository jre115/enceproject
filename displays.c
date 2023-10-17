#include <avr/io.h>
#include "system.h"
#include "navswitch.h"
#include "nav.h"
#include "pacer.h"
#include "matrix.h"
#include "ir_uart.h"
#include "timer.h"
#include "led.h"
#include "displays.h"

#define PRESCALAR 1024
#define CPU_F 8000000

#define PSR_COUNTDOWN_TIME 750
#define YOUR_CHOICE_TIME 1500
#define RESULT_DISPLAY_TIME 3000

#define ROCK NORTH
#define PAPER EAST
#define SCISSORS WEST


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


void timed_display(void(*displayfunc)(void), uint16_t milliseconds, char* prevDir, char* other)
{
    uint16_t ticks = (milliseconds) * (CPU_F / PRESCALAR) / 1000;
    char direction;
    matrix_init();

    timer_init();
    while (timer_get() < ticks) {
        navswitch_update();
        
        if (ir_uart_read_ready_p()) {
            *other = ir_uart_getc();
        }
        direction = direction_moved();
        if (direction == NORTH || direction == EAST || direction == WEST) {
            *prevDir = direction;
            ir_uart_putc(*prevDir);
            led_set(LED1, 1); // Blue LED on when choice made
        }
        displayfunc();
    }
    matrix_init();
}


void scrolling_text(char* text)
{
    matrix_init_text(text);
    show_display(&matrix_disp_text, ANY);

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


void icon_countdown(char prevDir, char other) 
{
    timed_display(&display_paper, PSR_COUNTDOWN_TIME, &prevDir, &other);
    timed_display(&display_scissors, PSR_COUNTDOWN_TIME, &prevDir, &other);
    timed_display(&display_rock, PSR_COUNTDOWN_TIME, &prevDir, &other);
    timed_display(&display_none, PSR_COUNTDOWN_TIME / 2, &prevDir, &other);
}


void display_own(char prevDir, char other)
{
    pacer_wait();
    if (prevDir == ROCK) {
        timed_display(&display_rock, YOUR_CHOICE_TIME, &prevDir, &other);
    } else if (prevDir == PAPER) {
        timed_display(&display_paper, YOUR_CHOICE_TIME, &prevDir, &other);
    } else if (prevDir == SCISSORS) {
        timed_display(&display_scissors, YOUR_CHOICE_TIME, &prevDir, &other);
    } else {
        timed_display(&display_none, YOUR_CHOICE_TIME, &prevDir, &other);
    }
}


void display_game_result(int8_t result, char prevDir, char other) 
{
    if (result == -1) {
        timed_display(&display_sad_face, RESULT_DISPLAY_TIME, &prevDir, &other);
    } else if (result == 0) {
        timed_display(&display_draw_face, RESULT_DISPLAY_TIME, &prevDir, &other);
    } else if (result == 1) {
        timed_display(&display_smiley_face, RESULT_DISPLAY_TIME, &prevDir, &other);
    }
}
