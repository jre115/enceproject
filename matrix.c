#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"

#include "matrix.h"
#include "navswitch.h"

#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2
#define PACER_RATE 500
#define MESSAGE_RATE 10

void init_matrix(void)
{
    pio_config_set(LEDMAT_ROW1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW3_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW4_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW5_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW6_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW7_PIO, PIO_OUTPUT_HIGH);

    pio_config_set(LEDMAT_COL1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL3_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL4_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL5_PIO, PIO_OUTPUT_HIGH);
}


int8_t disp_paper(void)
{
    uint16_t state = 0;

    while (1) {

        pacer_wait();
        init_matrix();
        navswitch_update ();

        if (navswitch_push_event_p(NAVSWITCH_WEST)) {
            return FROM_LOOP_EAST;  // Exit the loop when either west or east is pressed
        } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
            return FROM_LOOP_WEST;
        }
    
        if (state == 0) {
            pio_output_low(LEDMAT_ROW2_PIO);
            pio_output_low(LEDMAT_COL2_PIO);
            pio_output_low(LEDMAT_ROW3_PIO);
            pio_output_low(LEDMAT_ROW4_PIO);
            pio_output_low(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_ROW6_PIO);
            pio_output_low(LEDMAT_COL4_PIO);
            state = 1;
        } else {
            pio_output_low(LEDMAT_ROW2_PIO);
            pio_output_low(LEDMAT_ROW6_PIO);
            pio_output_low(LEDMAT_COL3_PIO);
            state = 0;
        }
    }
}

int8_t disp_rock(void)
{
    uint16_t state = 0;

    while (1) {

        pacer_wait();
        init_matrix();
        navswitch_update ();

        if (navswitch_push_event_p(NAVSWITCH_WEST)) {
            return FROM_LOOP_EAST;  // Exit the loop when either west or east is pressed
        } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
            return FROM_LOOP_WEST;
        }
    
        if (state == 0) {
            pio_output_low(LEDMAT_ROW3_PIO);
            pio_output_low(LEDMAT_ROW4_PIO);
            pio_output_low(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_COL3_PIO);
            state = 1;
        } else {
            pio_output_low(LEDMAT_ROW4_PIO);
            pio_output_low(LEDMAT_COL2_PIO);
            pio_output_low(LEDMAT_COL4_PIO);
            state = 0;
        }
    }
}


int8_t disp_scissors(void)
{
    uint16_t state = 0;

    while (1) {

        pacer_wait();
        init_matrix();
        navswitch_update ();

        if (navswitch_push_event_p(NAVSWITCH_WEST)) {
            return FROM_LOOP_EAST;  // Exit the loop when either west or east is pressed
        } else if (navswitch_push_event_p(NAVSWITCH_EAST)) {
            return FROM_LOOP_WEST;
        }
    
        if (state == 0) {
            pio_output_low(LEDMAT_ROW2_PIO);
            pio_output_low(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_ROW6_PIO);
            pio_output_low(LEDMAT_ROW7_PIO);
            pio_output_low(LEDMAT_COL1_PIO);
            pio_output_low(LEDMAT_COL5_PIO);
            state = 1;
        } else if (state == 1) {
            pio_output_low(LEDMAT_ROW3_PIO);
            pio_output_low(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_ROW6_PIO);
            pio_output_low(LEDMAT_COL2_PIO);
            pio_output_low(LEDMAT_COL4_PIO);
            state = 2;
        } else {
            pio_output_low(LEDMAT_ROW4_PIO);
            pio_output_low(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_COL3_PIO);
            state = 0;
        }
    }
}

void init_text(void)
{
    tinygl_init(PACER_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(MESSAGE_RATE);
}

/* Displays welcome message, to exit message shift nav stick*/
void disp_welcome(void)
{
    tinygl_text("Welcome to PSR! Move to start\0");
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);

    while(1)
    {
        pacer_wait();
        navswitch_update ();
        if (navswitch_push_event_p(NAVSWITCH_WEST) || navswitch_push_event_p(NAVSWITCH_EAST) || navswitch_push_event_p(NAVSWITCH_SOUTH) || navswitch_push_event_p(NAVSWITCH_NORTH)) {
            tinygl_clear();
            return;
        }
        
        tinygl_update();
    }
}
