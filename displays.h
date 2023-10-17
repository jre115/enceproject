#include "system.h"

void show_display(void(*displayfunc)(void), char direction);

void timed_display(void(*displayfunc)(void), uint16_t milliseconds, char* prevDir, char* other);

void scrolling_text(char* text);

void displayTutorial(void);

void icon_countdown(char prevDir, char other);

void display_own(char prevDir, char other);

void display_game_result(int8_t result, char prevDir, char other);

void disp_overall_result(char player);