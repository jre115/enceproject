#ifndef MATRIX_H
#define MATRIX_H

#include "system.h"

/* Initialise the matrix lights.  */
void matrix_init(void);

void matrix_display_shape(uint8_t* bitmap);

void matrix_init_text(char* text);

/* Displays set message*/
void matrix_disp_text(void);

void matrix_display_character (char character);

void matrix_display_rock(void);

void matrix_display_scissors(void);

void matrix_display_paper(void);

void matrix_display_north_arrow(void);

void matrix_display_east_arrow(void);

void matrix_display_west_arrow(void);

void matrix_display_none(void);

void matrix_display_smiley_face(void);

void matrix_display_sad_face(void);

void matrix_display_sand_timer(void);

void matrix_display_draw_face(void);

#endif //MATRIX_H
