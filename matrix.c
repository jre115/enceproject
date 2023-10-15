#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "tinygl.h"
#include "ledmat.h"
#include "navswitch.h"
#include "nav.h"

#include "../fonts/font5x7_1.h"

#include "matrix.h"

#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define MESSAGE_RATE 10
#define PACER_RATE 500

void matrix_init(void)
{
    ledmat_init();
    tinygl_clear();
}


void display_shape(uint8_t* bitmap)
{
    static uint8_t current_column = 1;
  
    ledmat_display_column (bitmap[current_column], current_column);

    current_column++;

    if (current_column > (LEDMAT_COLS_NUM - 1))
    {
        current_column = 0;
    }           
}


void init_text(char* text)
{
    tinygl_init(PACER_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text(text);
}

/* Displays set message*/
void disp_text(void)
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

/* Displays characters from given array, returns selected character*/
// char show_values(char* states, uint8_t n)
// {
//     uint8_t state = 0;
//     while (1) {
//         pacer_wait ();
//         tinygl_update ();
//         navswitch_update ();

//         if (navswitch_push_event_p(NAVSWITCH_EAST)) {
//             matrix_init();
//             state = (state + 1) % n;
//         } else if (navswitch_push_event_p(NAVSWITCH_WEST)) {
//             matrix_init();
//             state = (state - 1 + n) % n;
//         }

//         disp_character (states[state]);

//         if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
//             matrix_init();
//             return states[state];
//         }
//     }
// }

void display_rock(void)
{
    uint8_t rockmap[] = {0x00, 0x08, 0x1C, 0x08, 0x00};
    display_shape(rockmap);
}

void display_scissors(void)
{
    uint8_t scissormap[] = {0x27, 0x16, 0x0C, 0x16, 0x27};
    display_shape(scissormap);
}

void display_paper(void)
{
    uint8_t papermap[] = {0x7F, 0x41, 0x41, 0x41, 0x7F};
    display_shape(papermap);
}

void display_north_arrow(void)
{
    uint8_t bitmap[] = {0x04, 0x02, 0x7F, 0x02, 0x04};
    display_shape(bitmap);
}

void display_none(void)
{
    uint8_t bitmap[] = {0x0, 0x0, 0x0, 0x0, 0x0};
    display_shape(bitmap);
}

void display_east_arrow(void)
{
    uint8_t bitmap[] = {0x08, 0x08, 0x2A, 0x1C, 0x08};
    display_shape(bitmap);
}

void display_west_arrow(void)
{
    uint8_t bitmap[] = {0x08, 0x1C, 0x2A, 0x08, 0x08};
    display_shape(bitmap);
}