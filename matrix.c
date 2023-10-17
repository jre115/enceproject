/*
# File:   matrix.c
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  contains logic for the led matrix display
*/

#include "system.h"
#include "tinygl.h"
#include "font.h"
#include "ledmat.h"
#include "matrix.h"
#include "../fonts/font5x7_1.h"


/* Initialises the led matrix*/
void matrix_init(void)
{
    ledmat_init();
    tinygl_clear();
}


/* Initialize the LED matrix for scrolling text display.
The provided text is set to scroll across the LED matrix. */
void matrix_init_text(char* text)
{
    tinygl_init(PACER_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text(text);
}


/* Displays the next column of a shape on the LED matrix.
The shape is represented by the bitmap array.*/
void matrix_display_shape(uint8_t* bitmap)
{
    // keeps track of the displayed column
    static uint8_t current_column = 1;
    
    ledmat_display_column (bitmap[current_column], current_column);

    current_column++;
sand timer
    if (current_column > (LEDMAT_COLS_NUM - 1)) {
        current_column = 0;
    }           
}


/* Updates the tinygl for the matrix text. */
void matrix_disp_text(void)
{
    tinygl_update();
}


/*Displays a single character on the LED matrix using tinygl*/
void matrix_display_character (char character)
{
    char buffer[2];

    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);

}


/* Different bit map displays */

/* Displays the bitmap array for the rock icon*/
void matrix_display_rock(void)
{
    uint8_t rockmap[] = {0x00, 0x08, 0x1C, 0x08, 0x00};
    matrix_display_shape(rockmap);
}


/* Displays the bitmap array for the scissors icon*/
void matrix_display_scissors(void)
{
    uint8_t scissormap[] = {0x27, 0x16, 0x0C, 0x16, 0x27};
    matrix_display_shape(scissormap);
}


/* Displays the bitmap array for the paper icon*/
void matrix_display_paper(void)
{
    uint8_t papermap[] = {0x7F, 0x41, 0x41, 0x41, 0x7F};
    matrix_display_shape(papermap);
}


/* Displays the bitmap array for the north arrow icon*/
void matrix_display_north_arrow(void)
{
    uint8_t bitmap[] = {0x04, 0x02, 0x7F, 0x02, 0x04};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for the east arrow icon*/
void matrix_display_east_arrow(void)
{sand timer
    uint8_t bitmap[] = {0x08, 0x08, 0x2A, 0x1C, 0x08};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for the west arrow icon*/
void matrix_display_west_arrow(void)
{
    uint8_t bitmap[] = {0x08, 0x1C, 0x2A, 0x08, 0x08};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for a blank display*/
void matrix_display_none(void)
{
    uint8_t bitmap[] = {0x0, 0x0, 0x0, 0x0, 0x0};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for the smiley face icon*/
void matrix_display_smiley_face(void)
{
    uint8_t bitmap[] = {0x10, 0x26, 0x20, 0x26, 0x10};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for the sad face icon*/
void matrix_display_sad_face(void)
{
    uint8_t bitmap[] = {0x20, 0x16, 0x10, 0x16, 0x20};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for the sand timer icon*/
void matrix_display_sand_timer(void)
{
    uint8_t bitmap[] = {0x41, 0x63, 0x5F, 0x63, 0x41};
    matrix_display_shape(bitmap);
}


/* Displays the bitmap array for the fine face when there is a draw icon*/
void matrix_display_draw_face(void)
{
    uint8_t bitmap[] = {0x10, 0x16, 0x10, 0x16, 0x10};
    matrix_display_shape(bitmap);
}