/*
    Definitions for framebuffer class

*/

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_
#include "image.h"
#include "block.h"
#include "SDL2/include/SDL2/SDL.h"
#include <stdint.h>

#define P_HEIGHT 720
#define P_WIDTH 1280

extern uint32_t* framebuffer;   // should these be globals? Probably bad for performance...
extern SDL_Surface* f_surface;
extern unsigned int pitch;
void init_framebuffer(SDL_Surface* surface);
void drawPixel(int x, int y, uint32_t color);
void drawRect(int x1, int y1, int x2, int y2, uint32_t color, int fill);
void drawImage(int x, int y, int img_id);
void drawStaticImage(int x, int y, int img_id);
void drawRowImages(int x, int y, int num, int img_id);
void quickClear();
void fillWhite();
void drawArrayBlocks(BLOCK* blocks, int size);
void draw2DArrayBlocks(BLOCK** blocks, int rows, int cols);
#endif