#include "system.h"

#define PRESCALAR 1024
#define CPU_F 8000000

#define PSR_COUNTDOWN_TIME 750
#define YOUR_CHOICE_TIME 1500
#define RESULT_DISPLAY_TIME 3000

void displays_show_bitmap(void(*displayfunc)(void), char direction);

void displays_timed_display(void(*displayfunc)(void), uint16_t milliseconds, char* prevDir, char* other);

void displays_scrolling_text(char* text);

void displays_tutorial(void);

void displays_icon_countdown(char* prevDir, char* other);

void displays_own(char* prevDir, char* other);

void displays_game_result(int8_t result, char* prevDir, char* other);