#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "system.h"

#define PLAYER1 'A'
#define PLAYER2 'E'
#define PACER_RATE 500

char player1_player2(void);

char send_receive(char player, char message);

void wait(char player);

#endif // COMMUNICATION_H