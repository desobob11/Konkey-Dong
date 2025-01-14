/**
 * DEFINES BLOCK PROPERTIES
 *
 */

#ifndef BLOCK_H_
#define BLOCK_H_
#include "player.h"



#define BLOCK_WIDTH 32
#define BLOCK_HEIGHT 32

// x, y, and image id
typedef struct BLOCK {
    int x;
    int y;
    int img_id;
    int array_size;
} BLOCK;


int block_colliding_top(PLAYER* player, BLOCK* block);

int block_colliding_bottom(PLAYER* player, BLOCK* block);
int block_colliding_left(PLAYER *player, BLOCK *block);
int block_colliding_right(PLAYER *player, BLOCK *block);

void block_colliding_player(PLAYER* player, BLOCK* block);
void init_block(BLOCK* block, int x, int y, int img_id);

void init_block_array(BLOCK* bloc, int x, int y, int size, int img_id);
void init_block_column(BLOCK *bloc, int x, int y, int size, int img_id);

//void init_2d_block_array(BLOCK* bloc, int x, int y, int rows, int cols, int img_id);




#endif