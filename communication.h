#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "system.h"

#define PLAYER1 'A'
#define PLAYER2 'E'
#define PACER_RATE 500

char communication_player_setup(void);

char communication_send_and_recieve(char player, char message);

void communication_wait_for_other_player(char player);

#endif // COMMUNICATION_H