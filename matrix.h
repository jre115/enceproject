#ifndef MATRIX_H
#define MATRIX_H

#include "system.h"

/* Initialise the matrix lights.  */
void matrix_init(void);

void display_shape(uint8_t* bitmap);

void init_text(char* text);

/* Displays set message*/
void disp_text(void);

void disp_character (char character);

/* Displays characters from given array, returns selected character*/
// char selectVal(char* states, uint8_t n);

void display_rock(void);

void display_scissors(void);

void display_paper(void);

void display_north_arrow(void);

void display_east_arrow(void);

void display_west_arrow(void);

void display_none(void);

#endif //MATRIX_H
