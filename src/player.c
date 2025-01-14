#include "player.h"
#include "framebuffer.h"
#include "text.h"
#include "backup.h"
/**
 * Logic for the player
*/

/**
 * Draw the player's image
 * 
*/
void draw_player(PLAYER* player) {
    drawImage(player->x, player->y, player->img_id);
}


/**
 * 
 * Draw black over the player, not really used now
 * 
*/
void clear_player(PLAYER* player) {
    drawRect(player->x, player->y, player->x + SPRITE_WIDTH, player->y + SPRITE_HEIGHT, 0x00000000, 1);
}


/**
 * Efficiently redraw player when moving left,
 * only change pixels that have to be changed
 * 
*/
void draw_player_left(PLAYER* player) {
    int x = player->x;
    int y = player->y;

    for (int i = 0; i < SPRITE_HEIGHT; ++i) {
        for (int j = 0; j < PLAYER_X_SPEED; ++j) {
            drawPixel(x + 31 - j, y + i, backbuffer[x + 31 - j][y+ i]);
        }
    }
}

/**
 * Efficiently redraw player when moving right,
 * only change pixels that have to be changed
 *
 */
void draw_player_right(PLAYER* player) {
    int x = player->x;
    int y = player->y;

    for (int i = 0; i < SPRITE_HEIGHT; ++i) {
        for (int j = 0; j < PLAYER_X_SPEED; ++j) {
            drawPixel(x + j, y + i, backbuffer[x+j][y+i]);
        }
    }
}


/**
 * Efficiently redraw player when moving down,
 * only change pixels that have to be changed
 *
 */
void draw_player_down(PLAYER* player) {
    int x = player->x;
    int y = player->y;

    for (int i = 0; i < SPRITE_HEIGHT; ++i) {
        for (int j = 0; j < PLAYER_GRAVITY; ++j) {
            drawPixel(x + i, y + j, backbuffer[x + i][y + j]);
        }
    }
}

/**
 * Efficiently redraw player when moving up,
 * only change pixels that have to be changed
 *
 */
void draw_player_up(PLAYER *player)
{
    int x = player->x;
    int y = player->y;

    for (int i = 0; i < SPRITE_HEIGHT; ++i)
    {
        for (int j = 0; j < PLAYER_JUMP_SPEED; ++j)
        {
            drawPixel(x + i, y + 31 -j, backbuffer[x + i][y + 31 - j]);
        }
    }
}


/**
 * Function for moving  aplayer based on given input
 * 
 * 
*/
void move_player(PLAYER* player, SDL_Scancode code) {
    player->isMoving = 0;
    if (code == SDL_SCANCODE_W && player->y - PLAYER_CLIMB_SPEED > 0 + TEXT_HEIGHT && player->canClimbUp) {
        draw_player_up(player);
        player->y -= PLAYER_CLIMB_SPEED;
        draw_player(player);
        player->isMoving = 1;
    }
    if (code == SDL_SCANCODE_S && player->y + PLAYER_CLIMB_SPEED < P_HEIGHT - SPRITE_HEIGHT && player->canClimbDown)
    {
        draw_player_down(player);
        player->y += PLAYER_CLIMB_SPEED;
        draw_player(player);
        player->isMoving = 1;
    }
    if (code == SDL_SCANCODE_A && player->x > 0 && player->canMoveLeft)
    {
        draw_player_left(player);
        player->x -= PLAYER_X_SPEED;
        draw_player(player);
        player->isMoving = 1;
    }
    if (code == SDL_SCANCODE_D && player->x < P_WIDTH - SPRITE_WIDTH && player->canMoveRight)
    {
        draw_player_right(player);
        player->x += PLAYER_X_SPEED;
        draw_player(player);
        player->isMoving = 1;
    }
}

/**
 * Move player down if they are not on top of a platform
 * 
*/
void gravity(PLAYER* player) {
    if (player->canMoveDown && player->y < P_HEIGHT - SPRITE_HEIGHT && !player->isJumping) {

        draw_player_down(player);
        player->y += PLAYER_GRAVITY;
        draw_player(player);
    }
}

/**
 * Allow player to jump
 * 
*/
void jump_player(PLAYER* player) {
    if (!player->canMoveDown && player->canJump && !player->isJumping)
        {
            player->jumpLimit = player->y - JUMP_HEIGHT;
            player->isJumping = 1;
        }
    else if (player->isJumping) {
        if (player->y <= TEXT_HEIGHT) {
            player->isJumping = 0;
            return;
        }

        for (int i = player->x; i < player->x + SPRITE_HEIGHT; ++i)
        {
            for (int j = player->y; j < player->y + SPRITE_WIDTH; ++j)
            {
                    drawPixel(i, j, backbuffer[i][j]);
            }
        }


        player->y -= PLAYER_JUMP_SPEED;

        draw_player(player);
        if (player->y <= player->jumpLimit) {
            player->isJumping = 0;
        }
    }
}

int player_fell(PLAYER* player) {
    if (player->y >= P_HEIGHT - (2 * SPRITE_HEIGHT)) {
        Sleep(2000);
        return 1;
    }
    return 0;
}




