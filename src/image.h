#ifndef IMAGE_H_
#define IMAGE_H_
#define BLOCK_VOLCANO 0
#define BLOCK_WATERFALL 1
#define BANANA_SPRITE 2
#define BOULDER_IMG 3
#define BROWN_BLOCK 4
#define CLOCK_SPRITE 5
#define DK_TOKEN 6
#define GAME_OVER 7
#define GOAL 8
#define GOOMBA_LEFT 9
#define GOOMBA_RIGHT 10
#define LAVA 11
#define NUMBER_0 12
#define NUMBER_1 13
#define NUMBER_2 14
#define NUMBER_3 15
#define NUMBER_4 16
#define NUMBER_5 17
#define NUMBER_6 18
#define NUMBER_7 19
#define NUMBER_8 20
#define NUMBER_9 21
#define PAUSE 22
#define PLAYER_RIGHT 23
#define SAMPLE_TILE 24
#define SELECT 25
#define SPIDER 26
#define SPIKE 27
#define SWAMP 28
#define TEXT_LIVES 29
#define TEXT_SCORE 30
#define TEXT_TIME 31
#define TITLE 32
#define VINES 33
#define WATERFALL 34
#define WIN_GAME 35
struct Image {
int width;
int height;
 unsigned int* data;
};struct Tilemap {
int count;
struct Image images[];
};extern struct Tilemap tilemap;
#endif