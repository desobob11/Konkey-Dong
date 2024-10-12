#ifndef ENTITIES_H_
#define ENTITIES_H_
#include "player.h"
#include "framebuffer.h"
#include "text.h"
#include "backup.h"

#define BORG 0
#define ENEMY_WIDTH 32
#define ENEMY_HEIGHT 32
#define ENEMY_SPEED 2
#define GOAL_WIDTH 32
#define GOAL_HEIGHT 32

#define BOULDER_HEIGHT 32
#define BOULDER_WIDTH 32
#define BOULDER_GRAVITY 10
#define VINE_HEIGHT 32
#define VINE_WIDTH 32
#define SPIDER_SPEED 8

/**
 * CLASS TO DEFINE PROPERTIES OF ENTITIES
 * THESE SHOULD BE TYPES OF ENEMIES, ENEMY IS A GENERIC ENEMY TYPE FOR NOW (THINK GOOMBA)
 * 
*/




typedef struct ENEMY {
    int anchor_x;
    int left_target;
    int right_target;
    int direction;
    int x;
    int y;
    int type;
    int img_id;
    int speed;
    int drawable;
    int top_target;
    int bottom_target;
} ENEMY;


typedef struct BANANA {
    int x;
    int y;
    int img_id;
    int pad;
} BANANA;

typedef struct CLOCK
{
    int x;
    int y;
    int img_id;
    int pad;
} CLOCK;

typedef struct TOKEN
{
    int x;
    int y;
    int img_id;
    int spawned;
} TOKEN;

typedef struct GOALPOST {
    int x;
    int y;
} GOALPOST;

typedef struct BOULDER {
    int x;
    int y;
    int img_id;
    int is_active;
}   BOULDER;

typedef struct VINE {
    int x;
    int y;
    int img_id;
    int array_size;
} VINE;






void oscillate(ENEMY* e);

void clear_enemy(ENEMY* e);
void clear_token(TOKEN *e);
void clear_clock(CLOCK *e);
void clear_banana(BANANA *e);
void clear_boulder(BOULDER *e);
void draw_enemy(ENEMY* e);
void move_right(ENEMY* e);
void draw_enemy_right(ENEMY* e);
void draw_enemy_left(ENEMY* e);

void move_left(ENEMY* e);
int player_hit_enemy(PLAYER* player, ENEMY* enemy);
int player_take_damage(PLAYER* player, ENEMY* enemy, int size);
int player_reached_goal(PLAYER* player, GOALPOST* goal);
void drop_boulder(BOULDER* boulder);
void init_boulder(BOULDER* boulder, int img_id, int x);
int collide_banana(PLAYER* player, BANANA* banana);
int collide_boulder(PLAYER* player, BOULDER* boulder);
int collide_clock(PLAYER *player, CLOCK *clock);
int collide_token(PLAYER *player, TOKEN *clock);
void init_banana(BANANA*, int x, int y);
int collide_vine(PLAYER *player, VINE *vine);
void draw_banana(BANANA *banana);
void init_vines(VINE *vine, int x, int y, int size, int img_id);
void draw_vines(VINE *vines, int size);
void spider_climb_up(ENEMY* e);
void spider_climb_down(ENEMY* e);
void draw_enemy_up(ENEMY* e);
void draw_enemy_down(ENEMY* e);
void init_spider(ENEMY* e, int x, int y, int top, int bottom, int direction);
void climb_spider(ENEMY *e);
#endif