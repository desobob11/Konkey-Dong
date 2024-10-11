#include "entities.h"
#include "player.h"
#include "framebuffer.h"
#include "controller.h"
#include "text.h"
#include "backup.h"

/**
 * FILE DEFINES BEHAVIOR OF ENEMIES
 * 
 * 
*/




/**
 * IF PLAYER TOUCHES ENEMY, PLAYER DIES, WAITS 2 SECONDS, AND RETURNS TRUE
 * LEVEL WILL RESTART
 * 
*/
int player_hit_enemy(PLAYER* player, ENEMY* enemy) {
    int x_diff = player->x - enemy->x;
    if (x_diff < 0) x_diff *= -1;

    int y_diff = player->y - enemy->y;
    if (y_diff < 0) y_diff *= -1;

    if(x_diff <= SPRITE_WIDTH && y_diff <= SPRITE_HEIGHT) {
        wait(2000000);
        return 1;
    } 
    return 0;
}

/**
 * All collision detection gets the absolute difference of an enemy's x and y and a player's x and y coordinates
 * 
 * If the difference is less than the player's sprite width and/or height, there is a collision
*/

/**
 * PRETTY SELF EXPLANATORY
 * 
*/
int player_reached_goal(PLAYER* player, GOALPOST* goal) {
    int x_diff = player->x - goal->x;
    if (x_diff < 0) x_diff *= -1;

    int y_diff = player->y - goal->y;
    if (y_diff < 0) y_diff *= -1;

    return x_diff <= GOAL_WIDTH && y_diff <= GOAL_HEIGHT;

}

/*
    DEPRECATED - DRAWS A BLACK RECTANLG OVER OLD ENEMY SPRTIE LOCATION
*/
void clear_enemy(ENEMY* e) {
        for (int i = e->x; i < e->x + SPRITE_HEIGHT; ++i) {
            for (int j = e->y; j < e->y + SPRITE_WIDTH; ++j) {
                drawPixel(i, j, backbuffer[i][j]);
            }
        }
}

// erase a token
void clear_token(TOKEN *e)
{
        for (int i = e->x; i < e->x + SPRITE_HEIGHT; ++i)
        {
            for (int j = e->y; j < e->y + SPRITE_WIDTH; ++j)
            {
                drawPixel(i, j, backbuffer[i][j]);
            }
        }
}


// rease a boulder
void clear_boulder(BOULDER *e)
{
        for (int i = e->x; i < e->x + SPRITE_HEIGHT; ++i)
        {
            for (int j = e->y; j < e->y + SPRITE_WIDTH; ++j)
            {
                drawPixel(i, j, backbuffer[i][j]);
            }
        }
}


// erase a clock
void clear_clock(CLOCK *e)
{
        for (int i = e->x; i < e->x + SPRITE_HEIGHT; ++i)
        {
            for (int j = e->y; j < e->y + SPRITE_WIDTH; ++j)
            {
                drawPixel(i, j, backbuffer[i][j]);
            }
        }
}


// erase a banana
void clear_banana(BANANA *e)
{
        for (int i = e->x; i < e->x + SPRITE_HEIGHT; ++i)
        {
            for (int j = e->y; j < e->y + SPRITE_WIDTH; ++j)
            {
                drawPixel(i, j, backbuffer[i][j]);
            }
        }
}

/*
    WRAPPER FOR DRAWING AN EMEY GIVEN ITS COORDINATES

*/
void draw_enemy(ENEMY* e) {
    drawImage(e->x, e->y, e->img_id);
}


/*
    REWRITING ONLY UNCHANGED PIXELS WHEN MOVING LEFT
*/
void draw_enemy_left(ENEMY* e) {
        int x = e->x;
        int y = e->y;

        for (int i = 0; i < ENEMY_HEIGHT; ++i) {
            for (int j = 0; j < ENEMY_SPEED; ++j) {
                drawPixel(x + 31 - j, y + i, backbuffer[x + 31 - j][y + i]);
            }
        }
}

/*
    SAME FOR WHEN MOVING RIGHT
*/
void draw_enemy_right(ENEMY* e) {
    int x = e->x;
    int y = e->y;

    for (int i = 0; i < ENEMY_HEIGHT; ++i) {
        for (int j = 0; j < ENEMY_SPEED; ++j) {
            drawPixel(x + j, y + i, backbuffer[x + j][y + i]);
        }
    }
}

/**
 * Function for efficiently redrawing enemy
 * 
 * only redraws pixels that haven't been changed between
 * draws when moving down
 * 
 * 
*/
void draw_enemy_down(ENEMY* e)
{
    int x = e->x;
    int y = e->y;

    for (int i = 0; i < ENEMY_HEIGHT; ++i)
    {
        for (int j = 0; j < SPIDER_SPEED; ++j)
        {
            drawPixel(x + i, y + j, backbuffer[x + i][y + j]);
        }
    }
}

/**
 * Function for efficiently redrawing enemy
 *
 * only redraws pixels that haven't been changed between
 * draws when moving up
 *
 *
 */
void draw_enemy_up(ENEMY* e)
{
    int x = e->x;
    int y = e->y;

    for (int i = 0; i < ENEMY_HEIGHT; ++i)
    {
        for (int j = 0; j < SPIDER_SPEED; ++j)
        {
            drawPixel(x + i, y + 31 - j, backbuffer[x + i][y + 31 - j]);
        }
    }
}

/**
 * Function for efficiently redrawing enemy
 *
 * only redraws pixels that haven't been changed between
 * draws when moving right
 *
 *
 */
void move_right(ENEMY* e) {
    if (e->drawable) {
        if (e->x + ENEMY_WIDTH <= e->right_target && e->direction == 1) {
            draw_enemy_right(e);
            e->x += ENEMY_SPEED;
            draw_enemy(e);
        }
        else {
            e->direction = -1;
        }   
    }
}



/**
 * Function for efficiently redrawing enemy
 *
 * only redraws pixels that haven't been changed between
 * draws when moving left
 *
 *
 */
void move_left(ENEMY* e) {
    if (e->drawable) {
        if (e->x >= e->left_target && e->direction == -1) {
            draw_enemy_left(e);
            e->x -= ENEMY_SPEED;
            draw_enemy(e);
        }
        else {
            e->direction = 1;
        }   
    }
}

/**
 *  Function for climbing a spider up a vine
 *
 */
void spider_climb_up(ENEMY* e) {
    if (e->drawable)
    {
        // if stil room to move up
        if (e->y >= e->top_target && e->direction == -1)
        {
            // clear spider, update height, redraw
            clear_enemy(e);
            e->y -= SPIDER_SPEED;
            draw_enemy(e);
        }

        // else change direction to down
        else
        {
            e->direction = 1;
        }
    }
}


/**
 * Same as above but for moving a spider down
 * 
*/
void spider_climb_down(ENEMY *e)
{
    if (e->drawable)
    {
        if (e->y + ENEMY_HEIGHT <= e->bottom_target && e->direction == 1)
        {
            clear_enemy(e);
            e->y += SPIDER_SPEED;
            draw_enemy(e);
        }
        else
        {
            e->direction = -1;
        }
    }
}

/**
 * Function for moving enemies left to right
 * 
*/
void oscillate(ENEMY* e) {
    if (e->drawable) {
        // if still room to move right, move right
        if (e->direction == 1) {
            move_right(e);
        }

        // same for it moving left
        else if (e->direction == -1) {
            move_left(e);
        }
    }
}


/**
 * manage up and down climbing of spider
 * 
*/
void climb_spider(ENEMY* e) {
    if (e->drawable) {
        if (e->direction == 1) {
            spider_climb_down(e);
        }
        else  if (e->direction == -1) {
            spider_climb_up(e);
        }
    }
}

/**
 *  Iteate over an array of enemies and check for collision with player
 * 
*/
int player_take_damage(PLAYER* player, ENEMY* enemy, int size) {
    int check = 0;
    for (int i = 0; i < size; ++i) {
        if (player_hit_enemy(player, enemy + i)) {
            check = 1;
        }
    }
    return check;
}



/*
    Efficiently redraw boulder as it moves down the screen
*/
void draw_boulder_down(BOULDER* boulder) {
    int x = boulder->x;
    int y = boulder->y;

    for (int i = 0; i < BOULDER_HEIGHT; ++i) {
        for (int j = 0; j < BOULDER_GRAVITY; ++j) {
            drawPixel(x + i, y + j, backbuffer[x + i][y + j]);
        }
    }
}



/**
 * Move boulder down if possible
 * 
 * 
*/
void drop_boulder(BOULDER* boulder) {
    if (boulder->y + BOULDER_HEIGHT  + BOULDER_GRAVITY < P_HEIGHT - 32) {
        draw_boulder_down(boulder);
        boulder->y += BOULDER_GRAVITY;
        drawImage(boulder->x, boulder->y, boulder->img_id);
    }
    else {
        clear_boulder(boulder);
        init_boulder(boulder, boulder->img_id, boulder->x);
    }

}


/**
 * initialize a boulder
 * 
*/
void init_boulder(BOULDER* boulder, int img_id, int x) {
    boulder->x = x;
    boulder->y = TEXT_HEIGHT + 10;
    boulder->img_id = img_id;
    boulder->is_active = 1;
}


/**
 * check for collision with a banana
 * 
*/
int collide_banana(PLAYER *player, BANANA *banana) {
    int x_diff = player->x - banana->x;
    if (x_diff < 0)
        x_diff *= -1;

    int y_diff = player->y - banana->y;
    if (y_diff < 0)
        y_diff *= -1;
    // if they overlap clear the banana
    if (x_diff <= SPRITE_WIDTH && y_diff <= SPRITE_HEIGHT)
    {   clear_banana(banana);
        banana->x = 2000;
        banana->y = 2000;   
        return 1;
    }
    return 0;
}

/**
 *  banana collision
 * 
*/
int collide_boulder(PLAYER *player, BOULDER *boulder) {
    int x_diff = player->x - boulder->x;
    if (x_diff < 0)
        x_diff *= -1;

    int y_diff = player->y - boulder->y;
    if (y_diff < 0)
        y_diff *= -1;

    if (x_diff <= SPRITE_WIDTH && y_diff <= SPRITE_HEIGHT)
    {
        wait(2000000);
        return 1;
    }
    return 0;
}

/**
 * Clock collision
 * 
*/
int collide_clock(PLAYER *player, CLOCK* clock) {
    int x_diff = player->x - clock->x;
    if (x_diff < 0)
        x_diff *= -1;

    int y_diff = player->y - clock->y;
    if (y_diff < 0)
        y_diff *= -1;

    if (x_diff <= SPRITE_WIDTH && y_diff <= SPRITE_HEIGHT)
    {
        clear_clock(clock);
        clock->x = 2000;
        clock->y = 2000;
        return 1;
    }
    return 0;
}


/**
 * Token collision
 * 
*/
int collide_token(PLAYER *player, TOKEN *token) {
    int x_diff = player->x - token->x;
    if (x_diff < 0)
        x_diff *= -1;

    int y_diff = player->y - token->y;
    if (y_diff < 0)
        y_diff *= -1;

    if (x_diff <= SPRITE_WIDTH && y_diff <= SPRITE_HEIGHT)
    {
        clear_token(token);
        token->x = 2000;
        token->y = 2000;
        return 1;
    }
    return 0;
}

/*
    Returns one if player colliding, can be used to allow player to move up
    when colliding with vine

*/
int collide_vine(PLAYER* player, VINE* vine) {
    int x_diff = player->x - vine->x;
    if (x_diff < 0)
        x_diff *= -1;

    int y_diff = player->y - vine->y;
    if (y_diff < 0)
        y_diff *= -1;

    if (x_diff <= SPRITE_WIDTH / 2 && y_diff <= SPRITE_HEIGHT + VINE_HEIGHT && (button_held(BUTTON_UP) || button_held(BUTTON_DOWN)))
    {
        return 1;
    }
    return 0;
}

/**
 * Initialize a banana
 * 
*/
void init_banana(BANANA* banana, int x, int y) {
    banana->x = x;
    banana->y = y;
    banana->img_id = BANANA_SPRITE;
}

/**
 * Draw a banana
 * 
 * 
*/
void draw_banana(BANANA* banana) {
    drawImage(banana->x, banana->y, banana->img_id);
}


/**
 * Initialize an array of vines
 * 
 * 
*/
void init_vines(VINE* vine, int x, int y, int size, int img_id) {
    int cursor = y;
    for (int i = 0; i < size; ++i)
    {
        if (vine->y + VINE_HEIGHT >= P_HEIGHT) {
            (vine + i)->x = x;
            (vine + i)->y = y;
        }
        else {
            (vine + i)->x = x;
            (vine + i)->y = cursor;
        }
        (vine + i)->array_size = size;
        (vine + i)->img_id = img_id;
        cursor += BLOCK_HEIGHT;
    }
}


/**
 * Draw an array of vines
 * 
 * 
*/
void draw_vines(VINE* vines, int size)
{
    for (int i = 0; i < size; ++i)
    {
        drawStaticImage((vines + i)->x, (vines + i)->y, (vines + i)->img_id);
    }
}


/**
 * Initialize a spider
 * 
*/
void init_spider(ENEMY *e, int x, int y, int top_bound, int bottom_bound, int direction) {
    e->img_id = SPIDER;
    e->x = x;
    e->y = y;
    e->top_target = top_bound;
    e->bottom_target = bottom_bound;
    e->drawable = 1;
    e->direction = direction;
}

