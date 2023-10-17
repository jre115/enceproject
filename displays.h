/*
# File:   displays.h
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  contains display functions for the LED matrix
*/

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include "system.h"

#define PRESCALAR 1024
#define CPU_F 8000000

#define PSR_COUNTDOWN_TIME 750
#define YOUR_CHOICE_TIME 1500
#define RESULT_DISPLAY_TIME 3000


/**
 * Continuously displays a bitmap using 'display_function', exits when user navigates or goal direction reached.
 */
void displays_show_bitmap(void(*display_function)(void), char direction);


/**
 * Displays a custom marix display for a specific amount of time.
 */
void displays_timed_display(void(*display_function)(void), uint16_t milliseconds, char* previous_direction, char* other_players_direction);


/**
 * Scrolls the specified 'text' on the display screen and continuously displays it.
 */
void displays_scrolling_text(char* text);


/**
 * Displays a tutorial sequence on the screen, including scrolling text and corresponding
 * bitmaps for 'Rock', 'Paper', and 'Scissors', along with directional arrows.
 */
void displays_tutorial(void);


/**
 * Displays a countdown sequence with icons for 'Paper', 'Scissors', 'Rock', and 'None'.
 * It updates 'previous_direction' and 'other_players_direction' during the countdown.
 */
void displays_icon_countdown(char* previous_direction, char* other_players_direction);


/**
 * Displays your own choice icon based on 'previous_direction' and updates it during
 * the specified time. Also communicates with other players using 'other_players_direction'.
 */
void displays_own(char* previous_direction, char* other_players_direction);


/**
 * Displays a game result icon based on the 'result' parameter, updating it during
 * the specified time. Uses 'previous_direction' and 'other_players_direction' for
 * communication.
 */
void displays_game_result(int8_t result, char* previous_direction, char* other_players_direction);


/**
 * Displays the overall game result, with the player's score and the opponent's score.
 * Scrolls a message indicating the winner, loser, or a draw based on the scores.
 */
void displays_overall_result(char playerScoreAsChar, char otherScore);

#endif //DISPLAYS_H
