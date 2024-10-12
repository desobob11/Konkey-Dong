// ADAPTED FROM fb.c at https://github.com/isometimes/rpi4-osdev/tree/master/part5-framebuffer
// note: the resolution is very high and takes a long time to draw the whole screen!
// consider changing it :) 

/**
 * MOST FUNCTIONS PROVIDED IN BOILERPLATE CODE
 * 
 * 
*/

#include "framebuffer.h"
#include "image.h"
#include "block.h"
#include "backup.h"

unsigned int width, height, pitch;
uint32_t* framebuffer;
SDL_Surface* f_surface; // should these be globals? Probably bad for performance...



void init_framebuffer(SDL_Surface* surface)
{
    f_surface = surface;
    framebuffer = surface->pixels;
    SDL_LockSurface(f_surface);
    SDL_memset(framebuffer, 0x00000000, f_surface->h * f_surface->w);
    SDL_UnlockSurface(f_surface);
}

/*

    Draw pixel with x,y and color
*/

// assumes surface is unlocked
void drawPixel(int x, int y, uint32_t color)
{

    
    SDL_LockSurface(f_surface);
    uint32_t offset = (y * f_surface->w) + x;
    framebuffer[offset] = color; // deref pixel at address, store color code in it!
    SDL_UnlockSurface(f_surface);
}

void fillScreen(uint32_t color)
{
    SDL_LockSurface(f_surface);
    SDL_memset(framebuffer, color, f_surface->h * f_surface->w);
    SDL_UnlockSurface(f_surface);
}

// fill is a bool that decides if you fill in the color
void drawRect(int x1, int y1, int x2, int y2, uint32_t color, int fill)
{
    int y=y1;
    SDL_LockSurface(f_surface);
    while (y <= y2) {
       int x=x1;
       while (x <= x2) {
        // conditionally render based on current color
	  if ((x == x1 || x == x2) || (y == y1 || y == y2)) drawPixel(x, y, color);
	  else if (fill) drawPixel(x, y, color);
          x++;
       }
       y++;
    }
    SDL_UnlockSurface(f_surface);
}

/**
 * Redraw every non-black pixel
 * 
*/
void quickClear() {
    SDL_LockSurface(f_surface);
    for (int i = 0; i < P_HEIGHT; ++i) {
        for (int j = 0; j  < P_WIDTH; ++j) {
                uint32_t offs = (i * f_surface->w) + j;
                if (framebuffer[offs] != 0x00000000) {      // needed to have optimized drawing on PI, maybe not so much now
                    framebuffer[offs] = 0x00000000;
                }
        }
    }
    SDL_UnlockSurface(f_surface);
}


/*
    Dynamic images, not drawn to the back frame buffer
*/
void drawImage(int x, int y, int img_id)
{
    struct Image* img = &tilemap.images[img_id];
    int w = img->width;
    int h = img->height;

    SDL_LockSurface(f_surface);
    for (int row = 0; row < h; row++)
    {
        for (int col = 0; col < w; col++)
        {
            // int color
            unsigned int color = img->data[row * w + col];
            if (color != 0x00cccccc) {
                // write the code to write a pixel at the right position
                drawPixel(x + col, y + row,color);
            }
        }
    }
    SDL_UnlockSurface(f_surface);
}


/*
    DRAWS TO FRAMEBUFFER AS WELL AS THE BACKUP FRAMEBUFFER
*/
void drawStaticImage(int x, int y, int img_id) {
    struct Image *img = &tilemap.images[img_id];
    int w = img->width;
    int h = img->height;
    SDL_LockSurface(f_surface);
    for (int row = 0; row < h; row++)
    {
        for (int col = 0; col < w; col++)
        {
            // int color
            uint32_t color = img->data[row * w + col];
            if (color != 0x00cccccc)
            {
                // write the code to write a pixel at the right position
                drawPixel(x + col, y + row, color);
                backbuffer[x + col][y + row] = color;
            }
        }
    }
    SDL_UnlockSurface(f_surface);
}

/**
 * Draw a row of images to the framberuffer and backup buffer
 * 
 *  // drawStaticImage handles surface locking
 * 
*/
void drawRowImages(int x, int y, int num, int img_id) {
    struct Image* p = &tilemap.images[img_id];

    int cursor = x;

    for (int i = 0; i < num; ++i) {
        drawStaticImage(cursor, y, img_id);
        cursor += p->width;
    }
    
}

/*
    MAKE SURE THAT NUM IS EQUAL TO SIZE OF BLOCK ARRAY THAT IS PASSED IN

*/
void drawArrayBlocks(BLOCK* blocks, int size) {
    for (int i = 0; i < size; ++i) {
        drawStaticImage((blocks + i)->x, (blocks + i)->y, (blocks + i)->img_id);
    }
}


void draw2DArrayBlocks(BLOCK** blocks, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        drawArrayBlocks(blocks[i], cols);
    }
}