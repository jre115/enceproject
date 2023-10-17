#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "system.h"

#define PLAYER1 'A'
#define PLAYER2 'E'
#define PACER_RATE 500


/* Sets up players by allowing them to select a role. First person to move the nav
stick will be player 1. Returns the current player*/
char communication_player_setup(void);


/*Sends a message from one player to another and waits to receive a response depending on player order*/
char communication_send_and_recieve(char player, char message);


/*Waits for the other player to signal readiness before starting the game*/
void communication_wait_for_other_player(char player);

#endif // COMMUNICATION_H