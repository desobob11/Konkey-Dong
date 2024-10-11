#ifndef PLAYER_H_
#define PLAYER_H_

#define PLAYER_GRAVITY 18
#define PLAYER_X_SPEED 12
#define PLAYER_JUMP_SPEED 18
#define PLAYER_CLIMB_SPEED 6
#define JUMP_HEIGHT 100


typedef struct PLAYER {
    int x;
    int y;
    int img_id;
    int isColliding;
    int isMoving;
    int canMoveUp;
    int canMoveDown;
    int canMoveLeft;
    int canMoveRight;
    int canClimbDown;
    int canClimbUp;
    int lives;
    int speed;
    int canJump;
    int playerCollidingTopBlock;
    int isJumping;
    int jumpLimit;
} PLAYER;

#define SPRITE_WIDTH 32
#define SPRITE_HEIGHT 32

void draw_player(PLAYER* player);
void move_player(PLAYER* player);
void clear_player(PLAYER* player);
void gravity(PLAYER* player);
void jump_player(PLAYER* player);
void draw_player_up(PLAYER* player);
void draw_player_down(PLAYER* player);
void draw_player_left(PLAYER* player);
void draw_player_right(PLAYER* player);

int player_fell(PLAYER* player);
void quick_clear(PLAYER *player);
void draw_player_down_up(PLAYER *player);
#endif