#ifndef TEXT_H_
#define TEXT_H_
#include "framebuffer.h"
#include "image.h"
#define TEXT_0_LIVES 0
#define TEXT_0_TIME 240
#define TEXT_0_SCORE 800
#define TEXT_WIDTH 21
#define TEXT_HEIGHT 32
#define NUMBER_0_LIVES 150
#define NUMBER_0_TIME 360
#define NUMBER_0_SCORE 950

/*
 * 
 *  VERY IMPORTANT!!!! DO NOT TRY TO WRITE NUMBERS THAT ARE GREATER THAN 6 DIGITS
 * 
 * DO NOT TRY TO WRITE NEGATIVE NUMBERS 
 * 
 * 
 *
*/
#define DIGIT_MAX 3

void draw_int(int x, int y, int num);
void clear_int(int x, int y);
void draw_bar();
void draw_lives(int num);
void draw_time(int num);
void draw_score(int num);













#endif