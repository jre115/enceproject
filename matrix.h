#ifndef MATRIX_H
#define MATRIX_H

#include "system.h"

#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define MESSAGE_RATE 20
#define PACER_RATE 500

/* Initialise the matrix lights.  */
void matrix_init(void);

void matrix_display_shape(uint8_t* bitmap);

void matrix_init_text(char* text);

/* Displays set message*/
void matrix_disp_text(void);

void disp_character (char character);

void display_rock(void);

void display_scissors(void);

void display_paper(void);

void display_north_arrow(void);

void display_east_arrow(void);

void display_west_arrow(void);

void display_none(void);

void display_smiley_face(void);

void display_sad_face(void);

void display_sand_timer(void);

void display_draw_face(void);

#endif //MATRIX_H
