#ifndef MATRIX_H
#define MATRIX_H

#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"


#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2

/* Initialise the matrix lights.  */
void init_matrix(void);

int8_t disp_paper(void);

int8_t disp_rock(void);

int8_t disp_rock(void);


int8_t disp_scissors(void);

void init_text(void);

void disp_welcome(void);

void dispTutorial(void);

void display_character(char);

char selectVal(char*, uint8_t);

#endif //MATRIX_H
