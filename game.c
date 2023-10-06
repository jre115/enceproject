#include <avr/io.h>
#include "system.h"
#include "pio.h"
#include "pacer.h"
#include "navswitch.h"
#define PACER_RATE  500/* Pacer loop - controls main loop */


void init_led(void)
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

void disp_paper(void)
{
    uint16_t state = 0;

    while (1) {
        pacer_wait();
        if (navswitch_push_event_p(NAVSWITCH_WEST) || navswitch_push_event_p(NAVSWITCH_EAST)) {
            break;  // Exit the loop when either west or east is pressed
        }

        if (((PIND & (1<<7)) != 0)) {
            init_led();
            break;
        }
    
        if (state == 0) {
            pio_output_low(LEDMAT_ROW2_PIO);
            pio_output_low(LEDMAT_COL2_PIO);
            pio_output_low(LEDMAT_ROW3_PIO);
            pio_output_low(LEDMAT_ROW4_PIO);
            pio_output_low(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_ROW6_PIO);
            pio_output_high(LEDMAT_COL3_PIO);
            pio_output_low(LEDMAT_COL4_PIO);
            state = 1;
        } else {
            pio_output_low(LEDMAT_ROW2_PIO);
            pio_output_high(LEDMAT_COL2_PIO);
            pio_output_high(LEDMAT_ROW3_PIO);
            pio_output_high(LEDMAT_ROW4_PIO);
            pio_output_high(LEDMAT_ROW5_PIO);
            pio_output_low(LEDMAT_ROW6_PIO);
            pio_output_low(LEDMAT_COL3_PIO);
            pio_output_high(LEDMAT_COL4_PIO);
            state = 0;
        }
    }

    init_led();
}

void disp_rock(void) 
{

    pio_output_low(LEDMAT_ROW4_PIO);
    pio_output_low(LEDMAT_ROW5_PIO);
    pio_output_low(LEDMAT_ROW6_PIO);
    pio_output_low(LEDMAT_COL2_PIO);
    pio_output_low(LEDMAT_COL3_PIO);
    pio_output_low(LEDMAT_COL4_PIO);


}

void disp_rock2(void) 
{

    pio_output_low(LEDMAT_ROW4_PIO);
    pio_output_low(LEDMAT_ROW5_PIO);
    pio_output_low(LEDMAT_COL2_PIO);
    pio_output_low(LEDMAT_COL3_PIO);
    pio_output_low(LEDMAT_COL4_PIO);


}


void init_button(void) 
{
    /* Initialise port to drive LED 1.  */
    DDRC |= (1 << 2); // set pin 2 to output
    DDRD &= ~(1 << 7); // set bit 7 to be zero
}

void button_led(void)
{
    /* Turn LED 1 on.  */
    // holding place for this stuff.. does not work obvs needs to be in a loop
    if ((PIND & (1<<7)) != 0) {
        PORTC |= (1 << 2); // turn on LED
    } else {
        PORTC &= ~(1 << 2); // turn off LED
    }
}

int main (void)
{
    system_init ();
    init_led();
    pacer_init(PACER_RATE);
    init_button();
    navswitch_init ();

    //uint8_t ledcounter=0;
    //uint8_t buttoncounter=0;
    uint8_t state = 0;
    uint8_t num_states = 3;
    char states[3] = {'R', 'P', 'S'}; 



    while(1)
    {
        pacer_wait();
        navswitch_update ();

        if (navswitch_push_event_p (NAVSWITCH_EAST)) {
            init_led();
            state = (state + 1) % num_states;
        }

        if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            init_led();
            state = (state - 1 + num_states) % num_states;
        }

        if (states[abs(state)] == 'P') {
            navswitch_update ();
            disp_paper();
        } else if (states[abs(state)] == 'R') {
            disp_rock();
        } else if (states[abs(state)] == 'S') {
            disp_rock2();
        }


        
        
        /*
        ledcounter++;
        if (ledcounter >= 2) {
            ledcounter = 0;
            disp_paper();
        }
        */

    }

}
