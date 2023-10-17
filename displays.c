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

#define ROCK NORTH
#define PAPER EAST
#define SCISSORS WEST


void displays_show_bitmap(void(*displayfunc)(void), char direction)
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


void displays_timed_display(void(*displayfunc)(void), uint16_t milliseconds, char* prevDir, char* other)
{
    uint16_t ticks = (milliseconds) * (CPU_F / PRESCALAR) / 1000;
    char direction;
    matrix_init();

    TCNT1 = 0;
    while (TCNT1 < ticks) {
        navswitch_update();

        if (TCNT1 % 2 == 0) {
            displayfunc();
        }
        
        if (ir_uart_read_ready_p()) {
            *other = ir_uart_getc();
        }
        direction = direction_moved();
        if (direction == NORTH || direction == EAST || direction == WEST) {
            *prevDir = direction;
            ir_uart_putc(*prevDir);
            led_set(LED1, 1); // Blue LED on when choice made
        }
    }
    matrix_init();
}


void displays_scrolling_text(char* text)
{
    matrix_init_text(text);
    displays_show_bitmap(&matrix_disp_text, ANY);

}


void displays_tutorial(void)
{
    displays_scrolling_text("Rock\0");
    displays_show_bitmap(&matrix_display_north_arrow, NORTH);
    displays_show_bitmap(&matrix_display_rock, NORTH);

    displays_scrolling_text("Paper\0");
    displays_show_bitmap(&matrix_display_east_arrow, EAST);
    displays_show_bitmap(&matrix_display_paper, EAST);

    displays_scrolling_text("Scissors\0");
    displays_show_bitmap(&matrix_display_west_arrow, WEST);
    displays_show_bitmap(&matrix_display_scissors, WEST);
}


void displays_icon_countdown(char* prevDir, char* other) 
{
    displays_timed_display(&matrix_display_paper, PSR_COUNTDOWN_TIME, prevDir, other);
    displays_timed_display(&matrix_display_scissors, PSR_COUNTDOWN_TIME, prevDir, other);
    displays_timed_display(&matrix_display_rock, PSR_COUNTDOWN_TIME, prevDir, other);
    displays_timed_display(&matrix_display_none, PSR_COUNTDOWN_TIME / 2, prevDir, other);
}


void displays_own(char* prevDir, char* other)
{
    pacer_wait();
    if (*prevDir == ROCK) {
        displays_timed_display(&matrix_display_rock, YOUR_CHOICE_TIME, prevDir, other);
    } else if (*prevDir == PAPER) {
        displays_timed_display(&matrix_display_paper, YOUR_CHOICE_TIME, prevDir, other);
    } else if (*prevDir == SCISSORS) {
        displays_timed_display(&matrix_display_scissors, YOUR_CHOICE_TIME, prevDir, other);
    } else {
        displays_timed_display(&matrix_display_none, YOUR_CHOICE_TIME, prevDir, other);
    }
}


void displays_game_result(int8_t result, char* prevDir, char* other) 
{
    if (result == -1) {
        displays_timed_display(&matrix_display_sad_face, RESULT_DISPLAY_TIME, prevDir, other);
    } else if (result == 0) {
        displays_timed_display(&matrix_display_draw_face, RESULT_DISPLAY_TIME, prevDir, other);
    } else if (result == 1) {
        displays_timed_display(&matrix_display_smiley_face, RESULT_DISPLAY_TIME, prevDir, other);
    }
}

void displays_overall_result(char playerScoreAsChar, char otherScore)
{
    if (playerScoreAsChar > otherScore) {
        char result[11] = "Winner: 0\0";
        result[8] = playerScoreAsChar;
        displays_scrolling_text(result);
    } else if (playerScoreAsChar < otherScore) {
        char result[10] = "Loser: 0\0";
        result[7] = playerScoreAsChar;
        displays_scrolling_text(result);
    } else {
        char result[8] = "Draw: 0\0";
        result[6] = playerScoreAsChar;
        displays_scrolling_text(result);
    }
}