#include "system.h"

#define PRESCALAR 1024
#define CPU_F 8000000

#define PSR_COUNTDOWN_TIME 750
#define YOUR_CHOICE_TIME 1500
#define RESULT_DISPLAY_TIME 3000

void show_display(void(*displayfunc)(void), char direction);

void timed_display(void(*displayfunc)(void), uint16_t milliseconds, char* prevDir, char* other);

void scrolling_text(char* text);

void displayTutorial(void);

void icon_countdown(char prevDir, char other);

void display_own(char prevDir, char other);

void display_game_result(int8_t result, char prevDir, char other);

void disp_overall_result(char player);