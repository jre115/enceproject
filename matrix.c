#include "system.h"
#include "tinygl.h"
#include "font.h"
#include "ledmat.h"
#include "matrix.h"
#include "../fonts/font5x7_1.h"

#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define MESSAGE_RATE 10
#define PACER_RATE 500


void matrix_init(void)
{
    ledmat_init();
    tinygl_clear();
}


void matrix_display_shape(uint8_t* bitmap)
{
    static uint8_t current_column = 1;
  
    ledmat_display_column (bitmap[current_column], current_column);

    current_column++;

    if (current_column > (LEDMAT_COLS_NUM - 1))
    {
        current_column = 0;
    }           
}


void matrix_init_text(char* text)
{
    tinygl_init(PACER_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text(text);
}

/* Displays set message*/

void matrix_disp_text(void)
{
    tinygl_update();
}

void disp_character (char character)
{
    char buffer[2];

    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);

}


/* Different bit map displays */

void display_rock(void)
{
    uint8_t rockmap[] = {0x00, 0x08, 0x1C, 0x08, 0x00};
    matrix_display_shape(rockmap);
}


void display_scissors(void)
{
    uint8_t scissormap[] = {0x27, 0x16, 0x0C, 0x16, 0x27};
    matrix_display_shape(scissormap);
}


void display_paper(void)
{
    uint8_t papermap[] = {0x7F, 0x41, 0x41, 0x41, 0x7F};
    matrix_display_shape(papermap);
}


void display_north_arrow(void)
{
    uint8_t bitmap[] = {0x04, 0x02, 0x7F, 0x02, 0x04};
    matrix_display_shape(bitmap);
}


void display_none(void)
{
    uint8_t bitmap[] = {0x0, 0x0, 0x0, 0x0, 0x0};
    matrix_display_shape(bitmap);
}


void display_east_arrow(void)
{
    uint8_t bitmap[] = {0x08, 0x08, 0x2A, 0x1C, 0x08};
    matrix_display_shape(bitmap);
}


void display_west_arrow(void)
{
    uint8_t bitmap[] = {0x08, 0x1C, 0x2A, 0x08, 0x08};
    matrix_display_shape(bitmap);
}


void display_smiley_face(void)
{
    uint8_t bitmap[] = {0x10, 0x26, 0x20, 0x26, 0x10};
    matrix_display_shape(bitmap);
}


void display_sad_face(void)
{
    uint8_t bitmap[] = {0x20, 0x16, 0x10, 0x16, 0x20};
    matrix_display_shape(bitmap);
}


void display_sand_timer(void)
{
    uint8_t bitmap[] = {0x41, 0x63, 0x5F, 0x63, 0x41};
    matrix_display_shape(bitmap);
}


void display_draw_face(void)
{
    uint8_t bitmap[] = {0x10, 0x16, 0x10, 0x16, 0x10};
    matrix_display_shape(bitmap);
}