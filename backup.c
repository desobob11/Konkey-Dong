/**
 * Logic for backup frame buffer, a separate 
 *  array of integers in memory to store static images
 * drawn to frame buffer
 * 
*/

#include "backup.h"


// actual buffer
uint32_t backbuffer[P_WIDTH][P_HEIGHT];

/**
 * Function to draw image to backup frame buffer
 * 
 * 
*/
void drawImageBackup(int x, int y, int img_id)
    {
        // iterate over image data
        struct Image *img = &tilemap.images[img_id];
        int w = img->width;
        int h = img->height;
        for (int row = 0; row < h; row++)
        {
            for (int col = 0; col < w; col++)
            {
                unsigned int color = img->data[row * w + col];

                // if image color is not 0x00cccccc (invisible in our game) draw it to backup buffer
                if (color != 0x00cccccc)
                {
                    // write the code to write a pixel at the right position
                    backbuffer[x + col][y + row] = color;
                }
            }
        }
    }

/**
 * Resets backup frame buffer to black
 * 
*/
void init_backup() {
    for (int i = 0; i < P_WIDTH; ++i) {
        for (int j = 0; j < P_HEIGHT; ++j) {
            backbuffer[i][j] = 0x00000000;
        }
    }
}
