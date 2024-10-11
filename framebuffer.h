/*
    Definitions for framebuffer class

*/

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_
#include "image.h"
#include "block.h"

#define P_HEIGHT 720
#define P_WIDTH 1280

extern unsigned char* framebuffer;
extern unsigned int pitch;
void init_framebuffer();
void drawPixel(int x, int y, unsigned int color);
void drawChar(unsigned char ch, int x, int y, unsigned char attr);
void drawString(int x, int y, char *s, unsigned char attr);
void drawRect(int x1, int y1, int x2, int y2, unsigned int color, int fill);
void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned char attr);
void clearScreen();
void drawImage(int x, int y, int img_id);
void drawStaticImage(int x, int y, int img_id);
void drawRowImages(int x, int y, int num, int img_id);
void quickClear();
void fillWhite();
void drawArrayBlocks(BLOCK* blocks, int size);
void draw2DArrayBlocks(BLOCK** blocks, int rows, int cols);
#endif