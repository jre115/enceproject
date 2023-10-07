#include <avr/io.h>
#include <stdlib.h>
#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#define PACER_RATE  500/* Pacer loop - controls main loop */
#define FROM_LOOP_EAST 1
#define FROM_LOOP_WEST 2



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

void disp_rock(void) 
{
    init_matrix();
    pio_output_low(LEDMAT_ROW4_PIO);
    pio_output_low(LEDMAT_ROW5_PIO);
    pio_output_low(LEDMAT_ROW6_PIO);
    pio_output_low(LEDMAT_COL2_PIO);
    pio_output_low(LEDMAT_COL3_PIO);
    pio_output_low(LEDMAT_COL4_PIO);

}

void disp_sissors(void) 
{
    init_matrix();
    pio_output_low(LEDMAT_ROW4_PIO);
    pio_output_low(LEDMAT_ROW5_PIO);
    pio_output_low(LEDMAT_COL2_PIO);
    pio_output_low(LEDMAT_COL3_PIO);
    pio_output_low(LEDMAT_COL4_PIO);

}


void init_blue_led(void) 
{
    /* Initialise port to drive LED 1.  */
    DDRC |= (1 << 2); // set pin 2 to output
    DDRD &= ~(1 << 7); // set bit 7 to be zero
}

void blue_led(void)
{
    /* Turn LED 1 on.  */
    // holding place for this stuff.. does not work obvs needs to be in a loop
    while(1) {
        if ((PIND & (1<<7)) != 0) {
            PORTC |= (1 << 2); // turn on LED
        } else {
            PORTC &= ~(1 << 2); // turn off LED
        }
    }
}

int main (void)
{
    system_init ();
    init_matrix();
    pacer_init(PACER_RATE);
    init_blue_led();
    navswitch_init ();

    uint8_t state = 0;
    uint8_t num_states = 3;
    char states[3] = {'R', 'P', 'S'}; 
    uint8_t switchDirection = 0;


    while(1) {
        pacer_wait();
        navswitch_update ();

        if (navswitch_push_event_p(NAVSWITCH_EAST) || switchDirection == FROM_LOOP_EAST) {
            init_matrix();
            state = (state + 1) % num_states;
            switchDirection = 0;
        } else if (navswitch_push_event_p(NAVSWITCH_WEST) || switchDirection == FROM_LOOP_WEST) {
            init_matrix();
            state = (state - 1 + num_states) % num_states;
            switchDirection = 0;
        }

        if (states[abs(state)] == 'P') {
            switchDirection = disp_paper();
        } else if (states[abs(state)] == 'R') {
            disp_rock();
        } else if (states[abs(state)] == 'S') {
            navswitch_update ();
            disp_sissors();
        }

    }

}
