/*
# File:   matrix.h
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  contains logic for the led matrix display
*/

#ifndef MATRIX_H
#define MATRIX_H

#include "system.h"

#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define MESSAGE_RATE 20
#define PACER_RATE 500


/* Initialises the led matrix*/
void matrix_init(void);


/* Initialize the LED matrix for scrolling text display.
The provided text is set to scroll across the LED matrix. */
void matrix_init_text(char* text);


/* Displays the next column of a shape on the LED matrix.
The shape is represented by the bitmap array.*/
void matrix_display_shape(uint8_t* bitmap);


/* Updates the tinygl for the matrix text. */
void matrix_disp_text(void);


/*Displays a single character on the LED matrix using tinygl*/
void matrix_display_character (char character);


/* Displays the bitmap array for the rock icon*/
void matrix_display_rock(void);


/* Displays the bitmap array for the scissors icon*/
void matrix_display_scissors(void);


/* Displays the bitmap array for the paper icon*/
void matrix_display_paper(void);


/* Displays the bitmap array for the north arrow icon*/
void matrix_display_north_arrow(void);


/* Displays the bitmap array for the east arrow icon*/
void matrix_display_east_arrow(void);


/* Displays the bitmap array for the west arrow icon*/
void matrix_display_west_arrow(void);


/* Displays the bitmap array for a blank display*/
void matrix_display_none(void);


/* Displays the bitmap array for the smiley face icon*/
void matrix_display_smiley_face(void);


/* Displays the bitmap array for the sad face icon*/
void matrix_display_sad_face(void);


/* Displays the bitmap array for the sand timer icon*/
void matrix_display_sand_timer(void);


/* Displays the bitmap array for the fine face when there is a draw icon*/
void matrix_display_draw_face(void);

#endif //MATRIX_H
