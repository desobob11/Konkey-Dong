
/**
 * SOURCE FOR BLOCK.H
 * DEFINES BLOCK BEHAVIOR
 *
 * A BLOCK IS ANY PLATFORM
 *
 */


#include "block.h"
#include "player.h"
#include "framebuffer.h"




/**
 * Check to see if colliding on top of block
 * 
*/
int block_colliding_top(PLAYER* player, BLOCK* block) {

    // get x-difference, if less than player's width there is overlap along x-axis
    int x_diff = player->x + PLAYER_X_SPEED - block->x;
    if (x_diff < 0) x_diff *= -1;

    // If overlapping along x axis and y axis while player is above block, player cant move down
    if (x_diff < SPRITE_WIDTH  && player->y + SPRITE_HEIGHT + PLAYER_GRAVITY >= block->y && block->y + BLOCK_HEIGHT > player->y) {
        return 1;
    }
    return 0; 
}

/**
 * LOGIC IS THE SAME FOR OTHER COLLISION FUNCTIONS
 * 
 * 
*/
int block_colliding_bottom(PLAYER* player, BLOCK* block) {
    int x_diff = player->x - block->x;
    if (x_diff < 0) x_diff *= -1;

    if (x_diff <= SPRITE_WIDTH  && player->y <= block->y + BLOCK_HEIGHT + PLAYER_JUMP_SPEED && block->y <= player->y) {
        return 1;
    } 
    return 0;
}


/**
 * Check for player colliding from the right side
 * 
*/
int block_colliding_right(PLAYER* player, BLOCK* block) {
    int y_diff = player->y - block->y;
    if (y_diff < 0) y_diff *= -1;

    if (y_diff <= SPRITE_HEIGHT  && player->x <= block->x + BLOCK_WIDTH && block->x < player->x) {
        return 1;
    }
    return 0;
}

/**
 * Check for player colliding from the left side
 *
 */
int block_colliding_left(PLAYER* player, BLOCK* block) {
    int y_diff = player->y - block->y;
    if (y_diff < 0) y_diff *= -1;

    if (y_diff <= SPRITE_HEIGHT  && player->x + BLOCK_WIDTH >= block->x && block->x > player->x) {
        return 1;
    }
    return 0;
}


/**
 *  RUN ALL COLLISION FUNCTIONS AT ONCE
*/
void block_colliding_player(PLAYER* player, BLOCK* block) {
    block_colliding_left(player, block);
    block_colliding_right(player, block);
    block_colliding_top(player, block);
    block_colliding_bottom(player, block);

}





/*
    Set up block with x y and desired image
*/
void init_block(BLOCK* block, int x, int y, int img_id) {
    block->x = x;
    block->y = y;
    block->img_id = img_id;
} 

/*
    Do the same for an array of blocks
*/
void init_block_array(BLOCK* blocks, int x, int y, int size, int img_id) {
    int cursor = x;
    for (int i = 0; i < size; ++i) {
        (blocks + i)->x = cursor;
        (blocks + i)->y = y;
        (blocks + i)->img_id = img_id;
        (blocks + i)->array_size = size;
        cursor += BLOCK_WIDTH;
    }
}


// set up an array of blocks in a vertical platform
void init_block_column(BLOCK *blocks, int x, int y, int size, int img_id) {
    int cursor = y;
    for (int i = 0; i < size; ++i)
    {
        (blocks + i)->x = x;
        (blocks + i)->y = cursor;
        (blocks + i)->img_id = img_id;
        (blocks + i)->array_size = size;
        cursor += BLOCK_HEIGHT;
    }
}
