/*
# File:   communication.c
# Author: J. Redfern, R. Campbell-Stokes
# Date:   18 October 2023
# Descr:  Declares functions for player setup and communication using infrared
*/

#include "system.h"
#include "navswitch.h"
#include "pacer.h"
#include "ir_uart.h"
#include "matrix.h"
#include "nav.h"
#include "communication.h"


/* Sets up players by allowing them to select a role. First person to move the nav
stick will be player 1. Returns the current player*/
char communication_player_setup(void)
{
    char current = '0';
    uint8_t decided = FALSE; 
    char player = 'A';
    char sendChar = 'B';

    while (!decided) {
        navswitch_update();
        
        if (nav_direction_moved() != NO_DIRECTION) {
            decided = TRUE;
            ir_uart_putc(sendChar);
            current = PLAYER1;
        }

        if (ir_uart_read_ready_p()) {
            player = ir_uart_getc();
        } 
        
        if (player == 'B') {
            decided = TRUE;
            current = PLAYER2;
        }
    }

    return current;
}

/*Sends a message from one player to another and waits to receive a response depending on player order*/
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

/*Waits for the other player to signal readiness before starting the game*/
void communication_wait_for_other_player(char player)
{
    uint16_t tick = 0;
    char character;
    navswitch_init();
    uint8_t recev = FALSE;

    if (player == PLAYER1) {
        while(recev == 0) {
            // Increment the timer and display a sand timer animation for 'waiting'
            tick++;
            matrix_display_sand_timer();
            pacer_wait();

            if (tick > PACER_RATE) {
                tick = 0;
                ir_uart_putc(player);
            }

            // If a signal is received from PLAY/*Waits for the other player to signal readiness before starting the game*/
            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER2) {
                    recev = TRUE;
                }
            }
        }
        
    } else if (player == PLAYER2) {
        while (recev == FALSE) {
            matrix_display_sand_timer();
            pacer_wait();

            if (ir_uart_read_ready_p()) {
                character = ir_uart_getc();
                if (character == PLAYER1) {
                    recev = TRUE;
                    ir_uart_putc(player);
                }
            }
        }
    }
}