#include "system.h"
#include "navswitch.h"
#include "pacer.h"
#include "ir_uart.h"
#include "matrix.h"
#include "nav.h"
#include "communication.h"


char communication_player_setup(void)
{
    int current = 0;
    int decided = 0; 
    char player = 'A';
    char sendChar = 'B';

    while (!decided) {
        navswitch_update();
        
        if (direction_moved() != NO_DIRECTION) {
            decided = 1;
            ir_uart_putc(sendChar);
            current = PLAYER1;
        }

        if (ir_uart_read_ready_p()) {
            player = ir_uart_getc();
        } 
        
        if (player == 'B') {
            decided = 1;
            current = PLAYER2;
        }
    }

    return current;
}


char communication_send_and_recieve(char player, char message)
{
    char receivedMessage = NO_DIRECTION;
    if (player == PLAYER1) {
        ir_uart_putc(message);
        pacer_wait();
        receivedMessage = ir_uart_getc();
    } else if (player == PLAYER2) {
        receivedMessage = ir_uart_getc();
        pacer_wait();
        ir_uart_putc(message);
    }

    return receivedMessage;
}


void communication_wait_for_other_player(char player) {

    uint16_t tick = 0;
    char character;
    navswitch_init();
    uint8_t recev = 0;

    if (player == PLAYER1) {
        while(recev == 0) {
            tick++;
            matrix_display_sand_timer();
            pacer_wait();

            if (tick > PACER_RATE) {
                tick = 0;
                ir_uart_putc(player);
            }
    
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER2) {
                    recev = 1;
                }
            }
        }
        
    } else if (player == PLAYER2) {
        while (recev == 0) {
            matrix_display_sand_timer();
            pacer_wait();
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER1) {
                    recev = 1;
                    ir_uart_putc(player);
                }
            }
        }
    }
}