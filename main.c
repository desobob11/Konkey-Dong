/**
 * Main file
 * 
 * Contains functions for levels, menus, and main game loop
 * 
 * 
*/

#define SDL_MAIN_HANDLED
#include "SDL2/include/SDL2/SDL.h"
#include "framebuffer.h"
#include "image.h"
#include <stdio.h>

#include "player.h"
#include "entities.h"
#include "text.h"
#include "backup.h"
#include <time.h>
#include <windows.h>

// macro definitions
#define BAR_Y 325
#define START_X 750
#define SELECT_X 1050

#define BAR_HEIGHT 16
#define BAR_WIDTH 200
#define BAR_COLOR 0xffce00
#define WHITE 0xffffffff
#define BLACK 0x00000000
#define BLANK 0x00cccccc

#define RESTART_CODE -1
#define GAME_OVER_CODE -2



#define PAUSE_SCREEN_X P_WIDTH / 4
#define PAUSE_SCREEN_Y P_HEIGHT / 4
#define PAUSE_SCREEN_BOTTOM_X PAUSE_SCREEN_X + P_WIDTH / 2
#define PAUSE_SCREEN_BOTTOM_Y PAUSE_SCREEN_Y + P_HEIGHT / 2

#define RESTART_X PAUSE_SCREEN_X +  90
#define PAUSE_Y PAUSE_SCREEN_Y + 200
#define EXIT_X PAUSE_SCREEN_X +  400
#define GPIO_BASE 0xFE200000

/*
    Game State
*/
typedef struct GAME_STATE {
    int lives;
    int score;
    int time;
    time_t time_check;
    time_t clock;
    int paused;
    int restarted;
    int current_level_start_time;
    int current_level_start_score;
	unsigned short rngout;

} GAME_STATE;


// instantiate player and game state
GAME_STATE state;
PLAYER player;



/**
 * Function for printing game over screen
 * 
*/
void game_over(SDL_Window* wind) {
    // clear screen and print image
    quickClear();
    drawRect(0, 0, P_WIDTH, P_HEIGHT, BLACK, 1);
    drawImage(P_WIDTH / 2, P_HEIGHT / 2, GAME_OVER);

    // wait for input then return to main menu
    while (1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                return;
            }
        }
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }
}


/**
 * Function for printing victory screen
 * 
*/
void win_game(SDL_Window* wind)
{
    // clear scrren and print victory screen 
    quickClear();
    drawRect(0, 0, P_WIDTH, P_HEIGHT, BLACK, 1);
    drawImage(P_WIDTH / 2, P_HEIGHT / 2, WIN_GAME);

    // wait for input then return to main menu
    while (1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                return;
            }
        }
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }
}


/**
 *  Draw select bars on main menu 
 *
*/
void draw_title_bar(int x, int y) {
    drawImage(x, y, SELECT);
}

/**
 * Delete non-selected item's bar (delete yelllow)
 * 
*/
void clear_title_bar(int x, int y) {
    drawRect(x, y, x + BAR_WIDTH, y + BAR_HEIGHT, WHITE, 1);
}

/**
 * Function to exit game
 * 
*/
void no_start() {
    exit(0);
}


/*
   Draw pause menu
*/
void close_pause_menu() {
    // rewrite every pixel of the pause screen with the content of the backup frame buffer
    for (int i = PAUSE_SCREEN_X; i < PAUSE_SCREEN_BOTTOM_X; ++i) {
        for (int j = PAUSE_SCREEN_Y; j < PAUSE_SCREEN_BOTTOM_Y; ++j)
        {
            drawPixel(i, j, backbuffer[i][j]);
        }
    }
    draw_player(&player);
}

/**
 * Function to open pause menu
 * 
*/
int pause_menu(SDL_Window* wind) {
    // variable denotes that start or exit is selected
    int restart_exit = 0;

    // draw the pause image
    drawImage(P_WIDTH / 4, P_HEIGHT / 4, PAUSE);

    // wait one second
    Sleep(1000);

    // wait for input
    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                SDL_Scancode code = event.key.keysym.scancode;
                if (code == SDL_SCANCODE_ESCAPE) {
                    close_pause_menu();
                    return 0;
                }
                // if restart is selected
                if (!restart_exit) {
                    drawRect(EXIT_X, PAUSE_Y, EXIT_X + BAR_WIDTH, PAUSE_Y + BAR_HEIGHT, BLACK, 1);
                    drawRect(RESTART_X, PAUSE_Y, RESTART_X + BAR_WIDTH, PAUSE_Y + BAR_HEIGHT, WHITE, 1);

                    // if pressed right or left, choose exit optifon
                    if (code == SDL_SCANCODE_A || code == SDL_SCANCODE_D)
                    {
                        restart_exit = 1;
                    }

                    // if a is pressed restart level
                    if (code == SDL_SCANCODE_A)
                    {
                        state.restarted = 1;
                        return 1;
                    }
                }

                else {
                    // else exit is selected
                    drawRect(RESTART_X, PAUSE_Y, RESTART_X + BAR_WIDTH, PAUSE_Y + BAR_HEIGHT, BLACK, 1);
                    drawRect(EXIT_X, PAUSE_Y, EXIT_X + BAR_WIDTH, PAUSE_Y + BAR_HEIGHT, WHITE, 1);

                    // pressed right or left switch
                    if (code == SDL_SCANCODE_A || code == SDL_SCANCODE_D)
                    {
                        restart_exit = 0;
                    }

                    // if a exit to main menu
                    if (code == SDL_SCANCODE_A)
                    {
                        return -1;
                    }
                }
            }
        }
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }
    return -2;
}

// generate random number 0 or 1
unsigned short rng(){
	float next = (float) state.rngout;
    if (next == 0) {
        return 1;   
    }
    else {
        return (((int) (next*20.5))&0x7fff) + (((int) next)%2)*0x8000;
    }
	
}



/*
    setup game state for level 1
*/
void init_game_state(GAME_STATE* state) {
    time(&state->clock);
    state->lives = 4;
    state->score = 0;
    state->time = 100;
    state->time_check = state->clock;
    state->paused = 0;
    state->current_level_start_score = 0;
    state->current_level_start_time = 100;
	state->rngout = (unsigned short)state->clock;
}



/*
    setup player for level 1
*/
void init_player_level_1(PLAYER* player) {
    player->x = 0;
    player->y = P_HEIGHT - 100 - 32;
    player->img_id = PLAYER_RIGHT;
    player->canMoveDown = 0;
    player->canMoveLeft = 1;
    player->canMoveRight = 1;
    player->canMoveUp = 1;
    player->canClimbDown = 0;
    player->canClimbUp = 0;
    player->canJump = 1;
    player->isJumping = 0;
    player->jumpLimit = 0;
}


/*
    Setup player for level 3
*/
void init_player_level_3(PLAYER* player) {
    player->x = 0*32;
    player->y = 1*32;
    player->img_id = PLAYER_RIGHT;
    player->canMoveDown = 1;
    player->canMoveLeft = 1;
    player->canMoveRight = 1;
    player->canMoveUp = 1;
}

/*
    Title screen function
*/
void title_screen(SDL_Window* wind) {
    // start or exit selected
    int start_select = 0;

    // clear image draw title image
    quickClear();
    drawImage(0, 0, TITLE);

    // wait for input
    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                SDL_Scancode code = event.key.keysym.scancode;
                // is start selected and a pressed, continue into game loop
                if (!start_select) {
                    draw_title_bar(START_X, BAR_Y);
                    clear_title_bar(SELECT_X, BAR_Y);
                    if (code == SDL_SCANCODE_A || code == SDL_SCANCODE_D)
                    {
                        start_select = 1;
                    }
                    if (code == SDL_SCANCODE_SPACE || code == SDL_SCANCODE_ESCAPE)
                    {
                        return;
                    }
                }

                // if exit selected and a pressed exit to no_start screen 
                else {
                    draw_title_bar(SELECT_X, BAR_Y);
                    clear_title_bar(START_X, BAR_Y);
                    if (code == SDL_SCANCODE_A || code == SDL_SCANCODE_D)
                    {
                        start_select = 0;
                    }
                    if (code == SDL_SCANCODE_SPACE || code == SDL_SCANCODE_ESCAPE)
                    {
                        no_start();
                    }
                }
            }
        }
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }

}


/**
 * Function for level 1
 * 
 * 
*/
int level_1(SDL_Window* wind) {
    // setup player state for level 1
    init_player_level_1(&player);
    draw_player(&player);
    state.time = state.current_level_start_time;
    state.score = state.current_level_start_score;
	state.rngout = rng();

    // declare platforms
    BLOCK platform1[5];
    BLOCK platform2[5];
    BLOCK platform3[5];
    BLOCK platform4[5];
    BLOCK platform5[5];
    BLOCK platform6[5];
    BLOCK platform7[5];


    // allocate them in an arry
    BLOCK* allPlatforms[] = {platform1, platform2, platform3, platform4, platform5, platform6, platform7};
    int number_platforms = 7;

    // sizes
    int size_1 = sizeof(platform1) / sizeof(platform1)[0];
    int size_2 = sizeof(platform2) / sizeof(platform2)[0];
    int size_3 = sizeof(platform3) / sizeof(platform3)[0];
    int size_4 = sizeof(platform4) / sizeof(platform4)[0];
    int size_5 = sizeof(platform5) / sizeof(platform5)[0];
    int size_6 = sizeof(platform6) / sizeof(platform6)[0];
    int size_7 = sizeof(platform7) / sizeof(platform7)[0];

    // initialize struct data for each block
    init_block_array(platform1, 0, P_HEIGHT - 50 - 32, size_1, BROWN_BLOCK);
    init_block_array(platform2, 200, P_HEIGHT - 175, size_2, BROWN_BLOCK);
    init_block_array(platform3, 400, P_HEIGHT - 275, size_3, BROWN_BLOCK);
    init_block_array(platform4, 600, P_HEIGHT - 375, size_4, BROWN_BLOCK);
    init_block_array(platform5, 800, P_HEIGHT - 475, size_5, BROWN_BLOCK);
    init_block_array(platform6, 1000, P_HEIGHT - 575, size_6, BROWN_BLOCK);
    init_block_array(platform7, 0, P_HEIGHT - 275, size_7, BROWN_BLOCK);



    // Initialize banana1
    BANANA banana1;
    banana1.x = 0;
    banana1.y = platform7[0].y - BLOCK_HEIGHT;
    banana1.img_id = BANANA_SPRITE;

    // second random spawn
    if ((state.rngout % 2) == 1)
    {
        banana1.x = platform4[2].x;
        banana1.y = platform4[0].y - 150;
    }

    BANANA banana2;
    BANANA banana3;
    BANANA bananas[] = {banana1};
    int banana_count=  sizeof(bananas) / sizeof(bananas[0]);

    // initialize boulders
    BOULDER b1;
    BOULDER b3;
    init_boulder(&b1, BOULDER_IMG, 320);
    init_boulder(&b3, BOULDER_IMG, 960);
    BOULDER boulders[] = {b1, b3};
    int boulder_count = sizeof(boulders) / sizeof(boulders[0]);


    // GOAL POST INITIALIZATION
    GOALPOST goal;
    goal.x = 1100;
    goal.y = platform6[0].y - 32;


    // ENEMY INITIALIZATION
    ENEMY enemies[4];
   // ENEMY enemies[0];
    enemies[0].direction = 1;
    enemies[0].img_id = GOOMBA_RIGHT;
    enemies[0].y = platform2[0].y - 32;
    enemies[0].x = 260;
    enemies[0].left_target = 200;
    enemies[0].right_target = 360;
    enemies[0].drawable = 1;

    enemies[1].direction = -1;
    enemies[1].img_id = GOOMBA_RIGHT;
    enemies[1].y = platform3[0].y - 32;
    enemies[1].x = 460;
    enemies[1].left_target = 400;
    enemies[1].right_target = 560;
    enemies[1].drawable = 1;

    enemies[2].direction = 1;
    enemies[2].img_id = GOOMBA_RIGHT;
    enemies[2].y = platform4[0].y - 32;
    enemies[2].x = 660;
    enemies[2].left_target = 600;
    enemies[2].right_target = 760;
    enemies[2].drawable = 1;

    enemies[3].direction = -1;
    enemies[3].img_id = GOOMBA_RIGHT;
    enemies[3].y = platform5[0].y - 32;
    enemies[3].x = 860;
    enemies[3].left_target = 800;
    enemies[3].right_target = 960;
    enemies[3].drawable = 1;
    int enemy_count = sizeof(enemies) / sizeof(enemies[0]);

    // INITIALIZE CLOCK
    CLOCK clock;
    clock.x = platform2[2].x;
    clock.y = platform7[0].y - 100;
	state.rngout = rng();
	
	if ((state.rngout % 2) == 1) {
		clock.x = platform3[2].x;
		clock.y = platform4[0].y - 100;
	}

    // INITIALIZE TOKEN
    TOKEN token;
    token.x = 0;
    token.y = platform1[0].y - 100;
    token.spawned = 0;
	
	if ((state.rngout % 2) == 1) {
		token.x = 64;
		token.y = platform7[0].y - 100;
	}

    // initialize second token
    TOKEN token2;
    token2.x = platform6[2].x;
    token2.y = platform6[2].y - 32;
    token2.spawned = 1;

    // second possible spawn point
    if ((state.rngout % 2) == 1)
    {
        token2.x = platform2[2].x;
        token2.y = platform2[0].y - 100;
    }

    // DRAW ALL STATIC IMAGES (BLOCKS, SPIKED, BACKGROUND, GOAL, ETC)
    drawRowImages(0, P_HEIGHT - 32, 40, SPIKE);
    drawImage(goal.x, goal.y, GOAL);

    drawArrayBlocks(platform1, size_1);
    drawArrayBlocks(platform2, size_2);
    drawArrayBlocks(platform3, size_3);
    drawArrayBlocks(platform4, size_4);
    drawArrayBlocks(platform5, size_5);
    drawArrayBlocks(platform6, size_6);
    drawArrayBlocks(platform7, size_7);





    // GET CURRENT TIME FROM CLO BEFORE LEVEL LOOP STARTS
    time_t start_time;
    time(&start_time);

    // LEVEL LOOP
    int can_climb = 0;
    int colliding_top = 0;
    int colliding_bottom = 0;
    int colliding_left = 0;
    int colliding_right = 0;
    // LEVEL LOOP
    while (1) {
        
        time(&state.clock);
        // DECREMENT TIMER
        if (state.clock >= state.time_check) {
            --state.time;
            if (state.time == 0) {
                return -2;
            }
            state.time_check += 1;
        }

        time(&state.clock);
        // SPAWN TOKEN IF 30 SECONDS HAS PASSED
        if (state.clock - start_time > 10 && !token.spawned) {
            drawImage(token.x, token.y, DK_TOKEN);
            // DRAW ALL BANANAS
            for (int i = 0; i < banana_count; ++i)
            {
                drawImage(bananas[i].x, bananas[i].y, BANANA_SPRITE);
            }

            // CLOCK
            drawImage(clock.x, clock.y, CLOCK_SPRITE);

            // GOALPOST
            drawImage(token2.x, token2.y, DK_TOKEN);
            token.spawned = 1;
        }

        // CHECK FOR COLLISIONS WITH OBJECT AND ENTITIES
        for (int i = 0; i < boulder_count; ++i)
        {
            if (boulders[i].is_active)
            {
                drop_boulder(boulders + i);
            }
        }

        // check collision
        if (token.spawned)
        {
            if (collide_token(&player, &token) && token.spawned)
            {
                ++state.lives;
				if (state.lives >= 999){
					state.lives = 999;
				}
            }
        }

        // check collision
        if (token2.spawned)
        {
            if (collide_token(&player, &token2) && token.spawned)
            {
                //++state.lives;
				++state.lives;
				if (state.lives >= 999){
					state.lives = 999;
				}
            }
        }


        // check collision
        for (int i = 0; i < banana_count; ++i)
        {
            if (collide_banana(&player, &bananas[i]) && token.spawned)
            {
                state.score += 10;
				if (state.score >= 999){
					state.score = 999;
				}
            }
        }


        // check collision
        for (int i = 0; i < boulder_count; ++i)
        {
            if (collide_boulder(&player, &boulders[i]))
            {
                --state.lives;
                if (state.lives == 0)
                {
                    return -2;
                }
                return 0;
            }
        }


        // check collision
        if (collide_clock(&player, &clock) && token.spawned)
        {
            state.time += 10;
			if (state.time >= 999){
				state.time = 999;
			}
        }

        // move boulders
        for (int i = 0; i < boulder_count; ++i) {
            if (boulders[i].is_active) {
                drop_boulder(boulders + i);
            }
        }

        // check collision with enemies
        if (player_take_damage(&player, enemies, enemy_count))
        {
            --state.lives;
            if (state.lives == 0)
            {
                return -2;
            }
            return 0;
        }

        // check if reached goal
        if (player_reached_goal(&player, &goal))
        {
            state.current_level_start_score = state.score;
            state.current_level_start_time = state.time;
            return 1;
        }


        // check if player fell
        if (player_fell(&player))
        {
            --state.lives;
            if (state.lives == 0)
            {
                return -2;
            }
            return 0;
        }



        // UPDATE TEXT AT TOP
        draw_lives(state.lives);
        draw_time(state.time);
        draw_score(state.score);


        // check for collision with platforms
        for (int i = 0; i < number_platforms; ++i)
        {
            for (int j = 0; j < allPlatforms[i][0].array_size; ++j)
            {
                if (block_colliding_left(&player, &allPlatforms[i][j]))
                {
                    player.canMoveRight = 0;
                }
                if (block_colliding_right(&player, &allPlatforms[i][j]))
                {
                    player.canMoveLeft = 0;
                }
                if (block_colliding_bottom(&player, &allPlatforms[i][j]))
                {
                    player.canMoveUp = 0;
                    player.isJumping = 0;
                }

                // ADDITIONAL CHECK WHEN SEEING IF PLAYER IS COLLIDING WITH THE TOP OF A BLOCK (STANDING ON IT)

                // WHEN COLLIDING WITH A WALL, CHECKING COLLISION WITH THE TOP OF A BLOCK WILL MAKE THE PLAYER CLING
                // TO THE WALL. SO, IF WE ARE TOUCHING THE LEFT OR RIGHT SIDE OF A WALL/PLATFORM, WE CAN NEVER
                // BE COLLIDING WITH THE TOP OF IT, SO THE PLAYER WILL FALL
                // VERY IMPORTANT TO INCLUDE THE CONJUNCTION OF PLAYER BEING ABLE
                // TO MOVE LEFT AND RIGHT HERE
                if (block_colliding_top(&player, &allPlatforms[i][j]) && (player.canMoveLeft && player.canMoveRight))
                {
                    // player.playerCollidingTopBlock = 1;
                    player.canMoveDown = 0;
                }
            }
        }


        // move enemies left and right
        // UPDATE ENEMY POSITIONS AND REDRAW THEM
        oscillate(&enemies[0]);
        oscillate(&enemies[1]);
        oscillate(&enemies[2]);
        oscillate(&enemies[3]);


        // CHECCK FOR GRAVITY COLLISION WITH ANY PLATFORM FOR PLAYER (CAN BE POLISHED)
        // check for gravity collision
        for (int i = 0; i < size_1; ++i) {

            block_colliding_player(&player, &platform1[i]);
            if (!player.canMoveDown) break;
            block_colliding_player(&player, &platform2[i]);
            if (!player.canMoveDown) break;
            block_colliding_player(&player, &platform3[i]);
            if (!player.canMoveDown) break;
            block_colliding_player(&player, &platform4[i]);
            if (!player.canMoveDown) break;
            block_colliding_player(&player, &platform5[i]);
            if (!player.canMoveDown) break;
            block_colliding_player(&player, &platform6[i]);
            if (!player.canMoveDown) break;
            block_colliding_player(&player, &platform7[i]);
            if (!player.canMoveDown)
                break;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // check for clicking pause menu
            if (event.type == SDL_KEYDOWN) {
                SDL_Scancode code = event.key.keysym.scancode;
                if (code == SDL_SCANCODE_ESCAPE && !state.paused)
                {
                    // run pause menu function, if result is not 0 exit level with fail code 0
                    int pause_result = pause_menu(wind);
                    if (pause_result == 1)
                    {
                        return 0;
                    }
                    else if (pause_result == -1)
                    {
                        return -1;
                    }
                    time(&state.time_check);
                }
                else if (code == SDL_SCANCODE_SPACE) {
                    player.doJumpLoop = 1;
                } 
            
            // UPDATE PLAYER POSITION AND REDRAW PLAYER
            move_player(&player, code);
        }
    }
    gravity(&player);
    if (player.doJumpLoop)
        jump_player(&player);

    // reset collision detectors
    player.canMoveDown = 1;
    player.canClimbUp = 0;
    player.canClimbDown = 0;
    player.canMoveLeft = 1;
    player.canMoveRight = 1;
    player.doJumpLoop = 0;
    player.playerCollidingTopBlock = 0;
    colliding_top = 0;
    player.canJump = 1;
    SDL_UpdateWindowSurface(wind);
    SDL_Delay(1000 / 60);
    }
    // will never be reached put here for compiler warning purposes
    return 1;
}

/**
 * Function for level 3
 * 
 * 
 * 
*/

int level_3(SDL_Window* wind){
    // setup player andb state
    init_player_level_3(&player);
    draw_player(&player);
    state.time = state.current_level_start_time;
    state.score = state.current_level_start_score;
	state.rngout = rng();

    // Platform initalization
    BLOCK platform1[2];
    BLOCK platform2[3];
    BLOCK platform3[6];
    BLOCK platform4[11];
    BLOCK platform5[9];
    BLOCK platform6[3];
    BLOCK platform7[3];
    BLOCK platform8[3];
	BLOCK platform9[4];
    BLOCK platform10[6];
	BLOCK platform11[6];
    BLOCK platform12[2];
    BLOCK platform13[3];
    BLOCK platform14[3];
	BLOCK platform15[2];
	BLOCK platform16[3];
	BLOCK wall[15];
	

    // allocate all platforms in an array
	BLOCK* allPlatforms[] = {platform1, platform2, platform3, platform4, platform5, platform6,
                             platform7, platform8, platform9, platform10, platform11, platform12,
							 platform13, platform14, platform15, platform16, wall};

    // initialize all vines in an array
    VINE vines1[15];
    VINE vines2[7];
    VINE vines3[5];
    VINE vines4[4];
    int vines1_size = sizeof(vines1) / sizeof(vines1[0]);
    int vines2_size = sizeof(vines2) / sizeof(vines2[0]);
	int vines3_size = sizeof(vines3) / sizeof(vines3[0]);
    int vines4_size = sizeof(vines4) / sizeof(vines4[0]);

    init_vines(vines1, 0, 3*32, vines1_size, VINES);
    init_vines(vines2, 11*32, 3*32, vines2_size, VINES);
    init_vines(vines3, 19*32, 3*32, vines3_size, VINES);
    init_vines(vines4, 22*32, 14*32, vines4_size, VINES);

	VINE* allVines[] = {vines1, vines2, vines3, vines4};

    int number_vines = 4;

    int size_1 = sizeof(platform1) / sizeof(platform1)[0];
    int size_2 = sizeof(platform2) / sizeof(platform2)[0];
    int size_3 = sizeof(platform3) / sizeof(platform3)[0];
    int size_4 = sizeof(platform4) / sizeof(platform4)[0];
    int size_5 = sizeof(platform5) / sizeof(platform5)[0];
    int size_6 = sizeof(platform6) / sizeof(platform6)[0];
    int size_7 = sizeof(platform7) / sizeof(platform7)[0];
    int size_8 = sizeof(platform8) / sizeof(platform8)[0];
    int size_9 = sizeof(platform9) / sizeof(platform9)[0];
    int size_10 = sizeof(platform10) / sizeof(platform10)[0];
    int size_11 = sizeof(platform11) / sizeof(platform11)[0];
    int size_12 = sizeof(platform12) / sizeof(platform12)[0];
    int size_13 = sizeof(platform13) / sizeof(platform13)[0];
    int size_14 = sizeof(platform14) / sizeof(platform14)[0];
	int size_15 = sizeof(platform15) / sizeof(platform15)[0];
    int size_16 = sizeof(platform16) / sizeof(platform16)[0];
	int wallsize = sizeof(wall) / sizeof(wall)[0];
	
	int number_platforms = 17;
    int number_walls = 1;
	
    // initializes arrays of platforms
    init_block_array(platform1, 0, 3*32, size_1, BLOCK_VOLCANO);
    init_block_array(platform2, 2*32, 7*32, size_2, BLOCK_VOLCANO);
    init_block_array(platform3, 6*32, 10*32, size_3, BLOCK_VOLCANO);
    init_block_array(platform4, 11*32, 3*32, size_4, BLOCK_VOLCANO);
    init_block_array(platform5, 14*32, 18*32, size_5, BLOCK_VOLCANO);
    init_block_array(platform6, 10*32, 18*32, size_6, BLOCK_VOLCANO);
    init_block_array(platform7, 5*32, 18*32, size_7, BLOCK_VOLCANO);
	init_block_array(platform8, 0*32, 18*32, size_8, BLOCK_VOLCANO);
    init_block_array(platform9, 18*32, 8*32, size_9, BLOCK_VOLCANO);
    init_block_array(platform10, 23*32, 4*32, size_10, BLOCK_VOLCANO);
    init_block_array(platform11, 31*32, 3*32, size_11, BLOCK_VOLCANO);
    init_block_array(platform12, 38*32, 7*32, size_12, BLOCK_VOLCANO);
    init_block_array(platform13, 33*32, 10*32, size_13, BLOCK_VOLCANO);
    init_block_array(platform14, 28*32, 10*32, size_14, BLOCK_VOLCANO);
    init_block_array(platform15, 26*32, 9*32, size_15, BLOCK_VOLCANO);
    init_block_array(platform16, 22*32, 14*32, size_16, BLOCK_VOLCANO);
	init_block_column(wall, 18*32, 4*32, wallsize, BLOCK_VOLCANO);
    
    // initialize bananas
	BANANA banana1;
    banana1.x = 39*32;
    banana1.y = platform12[0].y - BLOCK_HEIGHT;
    banana1.img_id = BANANA_SPRITE;
    BANANA banana2;
	banana2.x = platform5[5].x;
    banana2.y = platform5[5].y - BLOCK_HEIGHT;
    banana2.img_id = BANANA_SPRITE;
    BANANA banana3;
	banana3.x = platform5[6].x;
    banana3.y = platform5[6].y - BLOCK_HEIGHT;
    banana3.img_id = BANANA_SPRITE;
	BANANA banana4;
    banana4.x = platform5[7].x;
    banana4.y = platform5[7].y - BLOCK_HEIGHT;
    banana4.img_id = BANANA_SPRITE;
    BANANA bananas[] = {banana1};
    int banana_count=  sizeof(bananas) / sizeof(bananas[0]);

    // initialize boulders
    BOULDER b;
    BOULDER b1;
    BOULDER b2;
    BOULDER b3;
    init_boulder(&b, BOULDER_IMG, 8*32);
    init_boulder(&b1, BOULDER_IMG, 22*32);
    init_boulder(&b2, BOULDER_IMG, 29*32);
    init_boulder(&b3, BOULDER_IMG, 2*32);
    BOULDER boulders[] = {b, b1, b2, b3};
    int boulder_count = sizeof(boulders) / sizeof(boulders[0]);
    
    // initialize goal post
	GOALPOST goal;
    goal.x = 23*32;
    goal.y = platform16[0].y - BLOCK_HEIGHT;
	
	// initialize enemies
    ENEMY enemies[5];
    enemies[0].direction = -1;
    enemies[0].img_id = GOOMBA_RIGHT;
    enemies[0].y = 6*32;
    enemies[0].x = 4*32;
    enemies[0].left_target = 2*32;
    enemies[0].right_target = 5*32;
    enemies[0].drawable = 1;

    enemies[1].direction = -1;
    enemies[1].img_id = GOOMBA_RIGHT;
    enemies[1].y = 9*32;
    enemies[1].x = 30*32;
    enemies[1].left_target = 28*32;
    enemies[1].right_target = 31*32;
    enemies[1].drawable = 1;

    enemies[2].direction = -1;
    enemies[2].img_id = GOOMBA_RIGHT;
    enemies[2].y = 9*32;
    enemies[2].x = 35*32;
    enemies[2].left_target = 33*32;
    enemies[2].right_target = 36*32;
    enemies[2].drawable = 1;

    enemies[3].direction = -1;
    enemies[3].img_id = GOOMBA_RIGHT;
    enemies[3].y = 17*32;
    enemies[3].x = 6*32;
    enemies[3].left_target = 5*32;
    enemies[3].right_target = 8*32;
    enemies[3].drawable = 1;

    enemies[4].direction = -1;
    enemies[4].img_id = GOOMBA_RIGHT;
    enemies[4].y = 17*32;
    enemies[4].x = 11*32;
    enemies[4].left_target = 10*32;
    enemies[4].right_target = 13*32;
    enemies[4].drawable = 1;
    int enemy_count = sizeof(enemies) / sizeof(enemies[0]);
	
	// initialize clock
    CLOCK clock;
    clock.x = platform9[2].x;
    clock.y = platform9[2].y - BLOCK_HEIGHT;

    //initialize token
    TOKEN token;
    token.x = platform5[3].x;
    token.y = platform5[3].y - BLOCK_HEIGHT;

    // possible second spawn point
	if ((state.rngout % 2) == 1) {
		token.x = 32;
		token.y = platform1[0].y - SPRITE_HEIGHT;
	}

    // draw all static blocks and platforms
	drawRowImages(0, P_HEIGHT - 32, 40,LAVA);
    drawArrayBlocks(platform1, size_1);
    drawArrayBlocks(platform2, size_2);
    drawArrayBlocks(platform3, size_3);
    drawArrayBlocks(platform4, size_4);
    drawArrayBlocks(platform5, size_5);
    drawArrayBlocks(platform6, size_6);
    drawArrayBlocks(platform7, size_7);
	drawArrayBlocks(platform8, size_8);
    drawArrayBlocks(platform9, size_9);
    drawArrayBlocks(platform10, size_10);
    drawArrayBlocks(platform11, size_11);
    drawArrayBlocks(platform12, size_12);
    drawArrayBlocks(platform13, size_13);
    drawArrayBlocks(platform14, size_14);
    drawArrayBlocks(platform15, size_15);
    drawArrayBlocks(platform16, size_16);
    drawArrayBlocks(wall, wallsize);
	
    // draw all static vines
	draw_vines(vines1, vines1_size);
    draw_vines(vines2, vines2_size);
    draw_vines(vines3, vines3_size);
	draw_vines(vines4, vines4_size);

    // draw goal ghost to screen
    drawStaticImage(goal.x, goal.y, GOAL);

    // BOOLEAN FOR TOKEN SPAWN
    int token_spawned = 0;

    // GET CURRENT TIME FROM CLO BEFORE LEVEL LOOP STARTS
    time_t start_time;
    time(&start_time);
	
	int can_climb = 0;
    int colliding_top = 0;
    int colliding_bottom = 0;
    int colliding_left = 0;
    int colliding_right = 0;
    
    // level loop
	while (1) {
        time(&state.clock);
        // decrement time counter every second
        if (state.clock >= state.time_check) {
            --state.time;
            if (state.time == 0)
            {
                return -2;
            }
            state.time_check += 1;
        }

        time(&state.clock);
        // spawn all value packs after 10 seconds
        if (state.clock - start_time > 10 && !token_spawned) {
            drawImage(token.x, token.y, DK_TOKEN);

            draw_banana(&banana1);
            draw_banana(&banana2);
            draw_banana(&banana3);
            draw_banana(&banana4);
            drawImage(clock.x, clock.y, CLOCK_SPRITE);

            token_spawned = 1;
        }

       // collision
        for (int i = 0; i < boulder_count; ++i)
        {
            if (boulders[i].is_active)
            {
                drop_boulder(boulders + i);
            }
        }

        // collision
        if (token_spawned)
        {
            if (collide_token(&player, &token) && token.spawned)
            {
                ++state.lives;
				if (state.lives >= 999){
					state.lives = 999;
				}
            }
        }

        //collision
        if (collide_banana(&player, &banana1) && token.spawned)
        {
            state.score += 10;
			if (state.score >= 999){
				state.score = 999;
			}
        }

        // collision
        if (collide_banana(&player, &banana2) && token.spawned)
        {
            state.score += 10;
			if (state.score >= 999){
				state.score = 999;
			}
        }

        // collision
        if (collide_banana(&player, &banana3) && token.spawned)
        {
            state.score += 10;
			if (state.score >= 999){
				state.score = 999;
			}
        }

        // collision
        if (collide_banana(&player, &banana4) && token.spawned)
        {
            state.score += 10;
			if (state.score >= 999){
				state.score = 999;
			}
        }

        // collision
        for (int i = 0; i < boulder_count; ++i)
        {
            if (collide_boulder(&player, &boulders[i]))
            {
                --state.lives;
                if (state.lives == 0)
                {
                    return -2;
                }
                return 0;
            }
        }

        // collision
        if (collide_clock(&player, &clock) && token.spawned)
        {
            state.time += 10;
			if (state.time >= 999){
				state.time = 999;
			}
        }
		
		// check player collision against enemies
        if (player_take_damage(&player, enemies, enemy_count))
        {
            --state.lives;
            if (state.lives == 0)
            {
                return -2;
            }
            return 0;
        }

        // check player reached end of level
        if (player_reached_goal(&player, &goal))
        {
            state.current_level_start_score = state.score;
            state.current_level_start_time = state.time;
            return 1;
        }
        if (player_fell(&player))
        {
            --state.lives;
            if (state.lives == 0)
            {
                return -2;
            }
            return 0;
        }


        // UPDATE TEXT AT TOP
        draw_lives(state.lives);
        draw_time(state.time);
        draw_score(state.score);
		
        // updatre enemy positions
		oscillate(&enemies[0]);
        oscillate(&enemies[1]);
        oscillate(&enemies[2]);
        oscillate(&enemies[3]);
		oscillate(&enemies[4]);
        oscillate(&enemies[5]);
        oscillate(&enemies[6]);
		
		// CHECCK FOR GRAVITY COLLISION WITH ANY PLATFORM FOR PLAYER (CAN BE POLISHED)
        for (int i = 0; i < number_platforms; ++i) {
            for (int j = 0; j < allPlatforms[i][0].array_size; ++j) {
                if (block_colliding_left(&player, &allPlatforms[i][j]))
                {
                    player.canMoveRight = 0;
                }
                if (block_colliding_right(&player, &allPlatforms[i][j]))
                {
                    player.canMoveLeft = 0;
                }
                if (block_colliding_bottom(&player, &allPlatforms[i][j])) {
                    player.canMoveUp = 0;
                    player.isJumping = 0;
                }
                
                // ADDITIONAL CHECK WHEN SEEING IF PLAYER IS COLLIDING WITH THE TOP OF A BLOCK (STANDING ON IT)

                // WHEN COLLIDING WITH A WALL, CHECKING COLLISION WITH THE TOP OF A BLOCK WILL MAKE THE PLAYER CLING
                // TO THE WALL. SO, IF WE ARE TOUCHING THE LEFT OR RIGHT SIDE OF A WALL/PLATFORM, WE CAN NEVER
                // BE COLLIDING WITH THE TOP OF IT, SO THE PLAYER WILL FALL
                // VERY IMPORTANT TO INCLUDE THE CONJUNCTION OF PLAYER BEING ABLE
                // TO MOVE LEFT AND RIGHT HERE
                if (block_colliding_top(&player, &allPlatforms[i][j]) && (player.canMoveLeft && player.canMoveRight))
                {
                    //player.playerCollidingTopBlock = 1;
                    player.canMoveDown = 0;
                }
            }
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                SDL_Scancode code = event.key.keysym.scancode;
                for (int i = 0; i < number_vines; ++i)
                {
                    for (int j = 0; j < allVines[i][0].array_size; ++j)
                    {
                        if (collide_vine(&player, &allVines[i][j], code))
                        {
                            player.canClimbUp = 1;
                            player.canClimbDown = 1;
                            player.canMoveDown = 0;
                            player.canJump = 0;
                        }
                    }
                }


                if (code == SDL_SCANCODE_ESCAPE && !state.paused)
                {
                    // run pause menu function, if result is not 0 exit level with fail code 0
                    int pause_result = pause_menu(wind);
                    if (pause_result == 1)
                    {
                        return 0;
                    }
                    else if (pause_result == -1)
                    {
                        return -1;
                    }
                    time(&state.time_check);
                    drawImage(clock.x, clock.y, CLOCK_SPRITE);
                    drawImage(goal.x, goal.y, GOAL);
                }
                else if (code == SDL_SCANCODE_SPACE) {
                    player.doJumpLoop = 1;
                }
            

            // UPDATE PLAYER POSITION AND REDRAW PLAYER
            move_player(&player, code);
            }
            if (player.doJumpLoop) {
                jump_player(&player);
            }
        }
        gravity(&player);
        if (player.doJumpLoop)
            jump_player(&player);
        

        player.canMoveDown = 1;
        player.canClimbUp = 0;
        player.canClimbDown = 0;
        player.canMoveLeft = 1;
        player.canMoveRight = 1;
        player.doJumpLoop = 0;
        player.playerCollidingTopBlock = 0;
        colliding_top = 0;
        player.canJump = 1;
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }
	return 1;
}

int level_2(SDL_Window* wind)
{
    // setup player state for level 1
    init_player_level_1(&player);
    draw_player(&player);
    state.time = state.current_level_start_time;
    state.score = state.current_level_start_score;
	state.rngout = rng();

    // OBJECT INITIALIZATION
    BLOCK platform1[10];    // bottom left
    BLOCK platform2[5];     // left middle
    BLOCK platform3[5];     // left top
    BLOCK platform4[5];     // middle left
    BLOCK platform5[5];     // middle
    BLOCK platform6[10];    // DK token
    BLOCK platform7[5];     // middle left
    BLOCK platform8[5];     // middle
    BLOCK platform9[1];     // middle left
    BLOCK platform10[1];     // middle
    BLOCK platform11[1];     // middle left
    BLOCK platform12[1];     // middle
    BLOCK wall1[5];
    BLOCK wall2[15];

    // SETUP ALL WALLS AND PLATFORMS IN A 2D ARRAY
    BLOCK* allPlatforms[] = {platform1, platform2, platform3, platform4, platform5, platform6,
                             platform7, platform8, platform9, platform10, platform11, platform12,wall1, wall2};


    // initialize vines
    VINE vines1[19];
    VINE vines2[19];
    VINE vines3[19];
    VINE vines4[19];
    VINE vines5[19];
    int vines_size = 19;

    init_vines(vines1, 160, 32, vines_size, VINES);
    init_vines(vines2, 268, 32, vines_size, VINES);
    init_vines(vines3, 582, 32, vines_size, VINES);
    init_vines(vines4, 693, 32, vines_size, VINES);
    init_vines(vines5, 868, 32, vines_size, VINES);



    // intialize spiders
    ENEMY spider1;
    ENEMY spider2;
    ENEMY spider3;
    ENEMY spider4;
    ENEMY spider5;
    init_spider(&spider1, vines1[0].x, vines1[0].y + SPRITE_HEIGHT, vines1[0].y + SPRITE_HEIGHT, vines1[18].y, 1);
    init_spider(&spider2, vines2[0].x, vines2[18].y, vines1[0].y + SPRITE_HEIGHT, vines2[18].y, -1);
    init_spider(&spider3, vines3[0].x, vines3[0].y + SPRITE_HEIGHT, vines1[0].y + SPRITE_HEIGHT, vines3[18].y, 1);
    init_spider(&spider4, vines4[0].x, vines4[18].y, vines1[0].y + SPRITE_HEIGHT, vines4[18].y, -1);
    init_spider(&spider5, vines5[0].x, vines5[0].y + SPRITE_HEIGHT, vines1[0].y + SPRITE_HEIGHT, vines5[18].y, 1);

    ENEMY allSpiders[] = {spider1, spider2, spider3, spider4, spider5};
    int number_enemies = sizeof(allSpiders) / sizeof(allSpiders[0]);


    // SET UP ALL VINES IN A 2D ARRAY
    VINE* allVines[] = {vines1, vines2, vines3, vines4, vines5};

    int number_vines = 5;


    int wall_size1 = 5;
    int wall_size2 = 15;


    int size_1 = sizeof(platform1) / sizeof(platform1[0]);
    int size_2 = sizeof(platform2) / sizeof(platform2[0]);
    int size_3 = sizeof(platform3) / sizeof(platform3[0]);
    int size_4 = sizeof(platform4) / sizeof(platform4[0]);
    int size_5 = sizeof(platform5) / sizeof(platform4[0]);
    int size_6 = sizeof(platform6) / sizeof(platform5[0]);
    int size_7 = sizeof(platform7) / sizeof(platform6[0]);
    int size_8 = sizeof(platform8) / sizeof(platform7[0]);
    int size_9 = sizeof(platform9) / sizeof(platform8[0]);
    int size_10 = sizeof(platform10) / sizeof(platform9[0]);
    int size_11 = sizeof(platform11) / sizeof(platform10[0]);
    int size_12 = sizeof(platform12) / sizeof(platform11[0]);
    int number_platforms = 14;
    int number_walls = 2;

    // initalize all platforms
    init_block_array(platform1, 0, P_HEIGHT - 50 - 32, size_1, BLOCK_WATERFALL);
    init_block_array(platform2, 0, P_HEIGHT - 350, size_2, BLOCK_WATERFALL);
    init_block_array(platform3, 0, P_HEIGHT - 550, size_3, BLOCK_WATERFALL);
    init_block_array(platform4, 300, P_HEIGHT - 350, size_4, BLOCK_WATERFALL);
    init_block_array(platform5, 300, P_HEIGHT - 550, size_5, BLOCK_WATERFALL);
    init_block_array(platform6, 800, P_HEIGHT - 50 - 32, size_6, BLOCK_WATERFALL);
    init_block_array(platform7, 900, P_HEIGHT - 350, size_7, BLOCK_WATERFALL);
    init_block_array(platform8, 900, P_HEIGHT - 550, size_8, BLOCK_WATERFALL);
    init_block_array(platform9, 550, P_HEIGHT - 350, size_9, BLOCK_WATERFALL);
    init_block_array(platform10, 725, P_HEIGHT - 350, size_10, BLOCK_WATERFALL);
    init_block_array(platform11, 550, P_HEIGHT - 600, size_11, BLOCK_WATERFALL);
    init_block_array(platform12, 725, P_HEIGHT - 600, size_12, BLOCK_WATERFALL);
    init_block_column(wall1, 460, 32, wall_size1, BLOCK_WATERFALL);
    init_block_column(wall2, 800, 32, wall_size2, BLOCK_WATERFALL);

    // initialize bananas
    BANANA banana1;
    init_banana(&banana1, 0, platform2[0].y - BLOCK_HEIGHT);
    BANANA banana2;
    init_banana(&banana2, 0, platform3[0].y - BLOCK_HEIGHT);
    BANANA banana3;
    init_banana(&banana3, banana2.x + SPRITE_WIDTH, banana2.y);
    BANANA banana4;
    init_banana(&banana4, banana3.x + SPRITE_WIDTH, banana2.y);
    BANANA banana5;
    init_banana(&banana5, banana4.x + SPRITE_WIDTH, banana2.y);
    BANANA bananas[] = {banana1, banana2, banana3, banana4, banana5};
    int number_bananas = sizeof(bananas) / sizeof(bananas[0]);

    // initialize tokens
    TOKEN token1;
    token1.x = wall1[0].x - SPRITE_WIDTH;
    token1.y = platform3[0].y - SPRITE_HEIGHT;
    token1.spawned = 0;
    token1.img_id = DK_TOKEN;

    // initialize tokens
    TOKEN token2;
    token2.x = platform12[0].x;
    token2.y = platform12[0].y - SPRITE_HEIGHT;
    token2.spawned = 1;
    token2.img_id = DK_TOKEN;
    
    // possible secondary spawn
	if ((state.rngout % 2) == 1) {
		token1.x = 550;
		token1.y = platform9[0].y - SPRITE_HEIGHT;
	}


    // GOAL POST INITIALIZATION
    GOALPOST goal;
    goal.x = 1028;
    goal.y = P_HEIGHT - 584;


    // initialize clocks
    CLOCK clock1;
    clock1.x = platform11[0].x;
    clock1.y = platform11[0].y - SPRITE_HEIGHT;
    clock1.img_id = CLOCK_SPRITE;

    CLOCK clock2;
    clock2.x = platform10[0].x;
    clock2.y = platform10[0].y - SPRITE_HEIGHT;
    clock2.img_id = CLOCK_SPRITE;

    CLOCK allClocks[] = {clock1, clock2};
    int number_clocks = sizeof(allClocks) / sizeof(allClocks[0]);

    // DRAW ALL STATIC IMAGES (BLOCKS, SPIKED, BACKGROUND, GOAL, ETC)
    drawRowImages(0, P_HEIGHT - 32, 40, WATERFALL);
    drawImage(goal.x, goal.y, GOAL);
    drawArrayBlocks(platform1, size_1);
    drawArrayBlocks(platform2, size_2);
    drawArrayBlocks(platform3, size_3);
    drawArrayBlocks(platform4, size_2);
    drawArrayBlocks(platform5, size_3);
    drawArrayBlocks(platform6, size_1);
    drawArrayBlocks(platform7, size_3);
    drawArrayBlocks(platform8, size_2);
    drawArrayBlocks(platform9, 1);
    drawArrayBlocks(platform10, 1);
    drawArrayBlocks(platform11, 1);
    drawArrayBlocks(platform12, 1);
    drawArrayBlocks(wall1, wall_size1);
    drawArrayBlocks(wall2, wall_size2);

    draw_vines(vines1, vines_size);
    draw_vines(vines2, vines_size);
    draw_vines(vines3, vines_size);
    draw_vines(vines4, vines_size);
    draw_vines(vines5, vines_size);



    // BOOLEAN FOR TOKEN SPAWN
    int token_spawned = 0;

    // GET CURRENT TIME FROM CLO BEFORE LEVEL LOOP STARTS
    time_t start_time;
    time(&start_time);

    // LEVEL LOOP
    int can_climb = 0;
    int colliding_top = 0;
    int colliding_bottom = 0;
    int colliding_left = 0;
    int colliding_right = 0;
    while (1)
    {
        time(&state.clock);
        // DECREMENT TIMER
        if (state.clock >= state.time_check)
        {
            --state.time;
            if (state.time == 0)
            {
                return -2;
            }
            state.time_check += 1;
        }

        time(&state.clock);
        // SPAWN TOKEN IF 30 SECONDS HAS PASSED
        if (state.clock - start_time > 10 && !token_spawned)
        {
            drawImage(token1.x, token1.y, token1.img_id);
            // DRAW ALL BANANAS
            for (int i = 0; i < number_bananas; ++i)
            {
                draw_banana(&bananas[i]);
            }

            // DRAW ALL CLOCKS
            for (int i = 0; i < number_clocks; ++i)
            {
                drawImage(allClocks[i].x, allClocks[i].y, allClocks[i].img_id);
            }

            drawImage(token2.x, token2.y, token2.img_id);
            token1.spawned = 1;
			token_spawned = 1;
        }

        /**
         * LOTS OF COLLISION CHECKS BELOW
         * 
         * 
        */
        if (token_spawned)
        {
            if (collide_token(&player, &token1) && token_spawned)
            {
				++state.lives;
				if (state.lives >= 999){
					state.lives = 999;
				}
			}
		}

        if (collide_token(&player, &token2) && token_spawned)
        {
            ++state.lives;
			if (state.lives >= 999){
				state.lives = 999;
			}
        }



        for (int i = 0; i < number_bananas; ++i)
        {
            if (collide_banana(&player, &bananas[i]) && token_spawned)
            {
                state.score += 10;
				if (state.score >= 999){
					state.score = 999;
				}
            }
        }

        for (int i = 0; i < number_clocks; ++i)
        {
            if (collide_clock(&player, &allClocks[i]) && token_spawned)
            {
                state.time += 10;
				if (state.time >= 999){
					state.time = 999;
				}
            }
        }

        if (player_take_damage(&player, allSpiders, number_enemies))
        {
              --state.lives;
              if (state.lives == 0)
              {
                return -2;
              }
            return 0;
        }

        if (player_reached_goal(&player, &goal)) {
            state.current_level_start_score = state.score;
            state.current_level_start_time = state.time;
            return 1;
        }


        if (player_fell(&player))
        {
            --state.lives;
            if (state.lives == 0)
            {
                return -2;
            }
            return 0;
        }






        // UPDATE TEXT AT TOP
        draw_lives(state.lives);
        draw_time(state.time);
        draw_score(state.score);


        /**
         * COLLISION DETECTION FOR PLATFORMS
         * 
         * 
         * 
        */
       
       // ITERATE OVER EACH PLATFORM AND CHECK FOR ALL TYPES OF COLLISION, SETTING PLAYER MOVEMENT FLAGS AS APPROPRIATE
        for (int i = 0; i < number_platforms; ++i) {
            for (int j = 0; j < allPlatforms[i][0].array_size; ++j) {
                if (block_colliding_left(&player, &allPlatforms[i][j]))
                {
                    player.canMoveRight = 0;
                }
                if (block_colliding_right(&player, &allPlatforms[i][j]))
                {
                    player.canMoveLeft = 0;
                }
                if (block_colliding_bottom(&player, &allPlatforms[i][j])) {
                    player.canMoveUp = 0;
                    player.isJumping = 0;
                }
                
                // ADDITIONAL CHECK WHEN SEEING IF PLAYER IS COLLIDING WITH THE TOP OF A BLOCK (STANDING ON IT)

                // WHEN COLLIDING WITH A WALL, CHECKING COLLISION WITH THE TOP OF A BLOCK WILL MAKE THE PLAYER CLING
                // TO THE WALL. SO, IF WE ARE TOUCHING THE LEFT OR RIGHT SIDE OF A WALL/PLATFORM, WE CAN NEVER
                // BE COLLIDING WITH THE TOP OF IT, SO THE PLAYER WILL FALL
                // VERY IMPORTANT TO INCLUDE THE CONJUNCTION OF PLAYER BEING ABLE
                // TO MOVE LEFT AND RIGHT HERE
                if (block_colliding_top(&player, &allPlatforms[i][j]) && (player.canMoveLeft && player.canMoveRight))
                {
                    //player.playerCollidingTopBlock = 1;
                    player.canMoveDown = 0;
                }
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                SDL_Scancode code = event.key.keysym.scancode;
                // Check and allow player to climb vines if they are in range
                for (int i = 0; i < number_vines; ++i)
                {
                    for (int j = 0; j < allVines[i][0].array_size; ++j)
                    {
                        if (collide_vine(&player, &allVines[i][j], code))
                        {
                            player.canClimbUp = 1;
                            player.canClimbDown = 1;
                            player.canMoveDown = 0;
                            player.canJump = 0;
                        }
                    }
                }

            
                // PAUSE MENU CHECK
                if (code == SDL_SCANCODE_ESCAPE && !state.paused)
                {
                    // run pause menu function, if result is not 0 exit level with fail code 0
                    int pause_result = pause_menu(wind);
                    if (pause_result == 1)
                    {
                        return 0;
                    }
                    else if (pause_result == -1)
                    {
                        return -1;
                    }
                    time(&state.time_check);
                    for (int i = 0; i < number_clocks; ++i)
                    {
                        drawImage(allClocks[i].x, allClocks[i].y, allClocks[i].img_id);
                    }
                }
                else if (code == SDL_SCANCODE_SPACE) {
                    player.doJumpLoop = 1;
                }
            
            // UPDATE PLAYER POSITION AND REDRAW PLAYER
            move_player(&player, code);
            }
        }
        gravity(&player);
        if (player.doJumpLoop)
            jump_player(&player);

        // climb spiders up down
        for (int i = 0; i < number_enemies; ++i) {
            climb_spider(&allSpiders[i]);
        }

        // RESTORE PLAYER MOVEMENT DEFAULTS
        player.canMoveDown = 1;
        player.canClimbUp = 0;
        player.canClimbDown = 0;
        player.canMoveLeft = 1;
        player.canMoveRight = 1;
        player.playerCollidingTopBlock = 0;
        colliding_top = 0;
        player.canJump = 1;
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }
    // will never be reached put here for compiler warning purposes
    return 1;
}

/**
 * Setup player for level 4
 * 
*/
void init_player_level_4(PLAYER *player)
{
    player->x = 0;
    player->y = 17 * 32;
    player->img_id = PLAYER_RIGHT;;
    player->canMoveDown = 0;
    player->canMoveLeft = 1;
    player->canMoveRight = 1;
    player->canMoveUp = 1;
    player->canClimbDown = 0;
    player->canClimbUp = 0;
    player->canJump = 1;
    player->isJumping = 0;
    player->jumpLimit = 0;
}

int level_4(SDL_Window* wind)
{
    // setup player and state for level
    init_player_level_4(&player);
    draw_player(&player);
    state.time = state.current_level_start_time;
    state.score = state.current_level_start_score;
	state.rngout = rng();

    // initialize platforms
    BLOCK platform1[11];
    BLOCK platform2[11];
    BLOCK platform3[11];
    BLOCK platform4[4];
    BLOCK platform5[6];
    BLOCK platform6[7];
    BLOCK platform7[2];
    BLOCK platform8[7];
    BLOCK platform9[7];
    BLOCK platform10[7];
    BLOCK platform11[6];
    BLOCK platform12[7];
    BLOCK platform13[7];
    BLOCK platform14[5];
    BLOCK platform15[5];

    BLOCK *allPlatforms[] = {platform1, platform2, platform3, platform4, platform5, platform6,
                             platform7, platform8, platform9, platform10, platform11, platform12,
                             platform13, platform14, platform15};


    // intitialize vines
    VINE vines1[7];
    VINE vines2[7];
    VINE vines3[7];
    VINE vines4[7];
    VINE vines5[7];
    VINE vines6[7];
    VINE vines7[7];
    VINE vines8[6];
    VINE vines9[6];
    VINE vines10[6];
    VINE vines11[6];
    VINE vines12[7];
    VINE vines13[5];
    VINE vines14[4];
    VINE vines15[3];
    int vines1_size = sizeof(vines1) / sizeof(vines1[0]);
    int vines2_size = sizeof(vines2) / sizeof(vines2[0]);
    int vines3_size = sizeof(vines3) / sizeof(vines3[0]);
    int vines4_size = sizeof(vines4) / sizeof(vines4[0]);
    int vines5_size = sizeof(vines5) / sizeof(vines5[0]);
    int vines6_size = sizeof(vines6) / sizeof(vines6[0]);
    int vines7_size = sizeof(vines7) / sizeof(vines7[0]);
    int vines8_size = sizeof(vines8) / sizeof(vines8[0]);
    int vines9_size = sizeof(vines9) / sizeof(vines9[0]);
    int vines10_size = sizeof(vines10) / sizeof(vines10[0]);
    int vines11_size = sizeof(vines11) / sizeof(vines11[0]);
    int vines12_size = sizeof(vines12) / sizeof(vines12[0]);
    int vines13_size = sizeof(vines13) / sizeof(vines13[0]);
    int vines14_size = sizeof(vines14) / sizeof(vines14[0]);
    int vines15_size = sizeof(vines15) / sizeof(vines15[0]);

    init_vines(vines1, 1 * 32, 12 * 32, vines1_size, VINES);
    init_vines(vines2, 4 * 32, 12 * 32, vines2_size, VINES);
    init_vines(vines3, 10 * 32, 12 * 32, vines3_size, VINES);
    init_vines(vines4, 15 * 32, 12 * 32, vines4_size, VINES);
    init_vines(vines5, 24 * 32, 12 * 32, vines5_size, VINES);
    init_vines(vines6, 30 * 32, 12 * 32, vines6_size, VINES);
    init_vines(vines7, 33 * 32, 12 * 32, vines7_size, VINES);
    init_vines(vines8, 39 * 32, 13 * 32, vines8_size, VINES);
    init_vines(vines9, 21 * 32, 6 * 32, vines9_size, VINES);
    init_vines(vines10, 18 * 32, 6 * 32, vines10_size, VINES);
    init_vines(vines11, 11 * 32, 6 * 32, vines11_size, VINES);
    init_vines(vines12, 3 * 32, 5 * 32, vines12_size, VINES);
    init_vines(vines13, 36 * 32, 6 * 32, vines13_size, VINES);
    init_vines(vines14, 39 * 32, 6 * 32, vines14_size, VINES);
    init_vines(vines15, 37 * 32, 3 * 32, vines15_size, VINES);


    int number_vines = 15;

    int size_1 = sizeof(platform1) / sizeof(platform1)[0];
    int size_2 = sizeof(platform2) / sizeof(platform2)[0];
    int size_3 = sizeof(platform3) / sizeof(platform3)[0];
    int size_4 = sizeof(platform4) / sizeof(platform4)[0];
    int size_5 = sizeof(platform5) / sizeof(platform5)[0];
    int size_6 = sizeof(platform6) / sizeof(platform6)[0];
    int size_7 = sizeof(platform7) / sizeof(platform7)[0];
    int size_8 = sizeof(platform8) / sizeof(platform8)[0];
    int size_9 = sizeof(platform9) / sizeof(platform9)[0];
    int size_10 = sizeof(platform10) / sizeof(platform10)[0];
    int size_11 = sizeof(platform11) / sizeof(platform11)[0];
    int size_12 = sizeof(platform12) / sizeof(platform12)[0];
    int size_13 = sizeof(platform13) / sizeof(platform13)[0];
    int size_14 = sizeof(platform14) / sizeof(platform14)[0];
    int size_15 = sizeof(platform15) / sizeof(platform15)[0];

    int number_platforms = 15;
    int number_walls = 0;

    // initialize platforms
    init_block_array(platform1, 0 * 32, 19 * 32, size_1, BLOCK_WATERFALL);
    init_block_array(platform2, 14 * 32, 19 * 32, size_2, BLOCK_WATERFALL);
    init_block_array(platform3, 29 * 32, 19 * 32, size_3, BLOCK_WATERFALL);
    init_block_array(platform4, 36 * 32, 13 * 32, size_4, BLOCK_WATERFALL);
    init_block_array(platform5, 29 * 32, 12 * 32, size_5, BLOCK_WATERFALL);
    init_block_array(platform6, 21 * 32, 12 * 32, size_6, BLOCK_WATERFALL);
    init_block_array(platform7, 17 * 32, 12 * 32, size_7, BLOCK_WATERFALL);
    init_block_array(platform8, 9 * 32, 12 * 32, size_8, BLOCK_WATERFALL);
    init_block_array(platform9, 0 * 32, 12 * 32, size_9, BLOCK_WATERFALL);
    init_block_array(platform10, 0 * 32, 5 * 32, size_10, BLOCK_WATERFALL);
    init_block_array(platform11, 9 * 32, 6 * 32, size_11, BLOCK_WATERFALL);
    init_block_array(platform12, 18 * 32, 6 * 32, size_12, BLOCK_WATERFALL);
    init_block_array(platform13, 26 * 32, 4 * 32, size_13, BLOCK_WATERFALL);
    init_block_array(platform14, 35 * 32, 6 * 32, size_14, BLOCK_WATERFALL);
    init_block_array(platform15, 35 * 32, 3 * 32, size_15, BLOCK_WATERFALL);


    // initialize bananas
    BANANA banana1;
    banana1.x = platform7[1].x;
    banana1.y = platform7[0].y -  BLOCK_HEIGHT;
    banana1.img_id = BANANA_SPRITE;
    BANANA bananas[] = {banana1};
    int banana_count = sizeof(bananas) / sizeof(bananas[0]);

    // initialize goal posts
    GOALPOST goal;
    goal.x = 35 * 32;
    goal.y = platform15[0].y - BLOCK_HEIGHT;

    // INITIALIZE TOKENs
    TOKEN token;
    token.x = 2 * 32;
    token.y = platform1[0].y - BLOCK_HEIGHT;


	if ((state.rngout % 2) == 1) {
		token.x = 27*32;
		token.y = platform6[0].y - SPRITE_HEIGHT;
	}


    // initalize enemies
    ENEMY spider1;
    ENEMY spider2;
    ENEMY spider3;
    ENEMY spider4;
    ENEMY spider5;
    ENEMY spider6;
    ENEMY spider7;

    init_spider(&spider1, vines3[0].x, platform1[10].y - BLOCK_HEIGHT, vines3[0].y - BLOCK_HEIGHT, platform1[10].y, 1);
    init_spider(&spider2, vines5[0].x, platform2[10].y - BLOCK_HEIGHT, vines5[0].y - BLOCK_HEIGHT, platform2[10].y, 1);
    init_spider(&spider3, vines7[0].x, platform3[4].y - BLOCK_HEIGHT, vines7[0].y - BLOCK_HEIGHT, platform3[4].y, 1);
    init_spider(&spider4, vines8[0].x, vines8[0].y, vines8[0].y - BLOCK_HEIGHT, platform3[10].y, 1);
    init_spider(&spider5, vines11[0].x, platform8[2].y - BLOCK_HEIGHT, vines11[0].y - BLOCK_HEIGHT, platform8[2].y, 1);
    init_spider(&spider6, vines12[0].x, platform9[3].y - BLOCK_HEIGHT, vines12[0].y - BLOCK_HEIGHT, platform9[3].y, 1);
    init_spider(&spider7, vines9[0].x, vines9[0].y, vines9[0].y - BLOCK_HEIGHT, platform6[0].y, 1);

    ENEMY allSpiders[] = {spider1, spider2, spider3, spider4, spider5, spider6, spider7};
    int number_enemies = 7;
    VINE *allVines[] = {vines1, vines2, vines3, vines4, vines5,
                        vines6, vines7, vines8, vines9, vines10,
                        vines11, vines12, vines13, vines14, vines15};

    // initalize clocks
    CLOCK clock;
    clock.x = platform2[6].x;
    clock.y = platform2[0].y - BLOCK_HEIGHT;
    clock.img_id = CLOCK_SPRITE;
	state.rngout = rng();
    
    // possible second spawn point
	if ((state.rngout % 2) == 1) {
		clock.x = platform5[3].x;
		clock.y = platform5[0].y - BLOCK_HEIGHT;
	}

    // draw all static platforms and images
    drawRowImages(0, P_HEIGHT - 32, 40, SWAMP);
    drawArrayBlocks(platform1, size_1);
    drawArrayBlocks(platform2, size_2);
    drawArrayBlocks(platform3, size_3);
    drawArrayBlocks(platform4, size_4);
    drawArrayBlocks(platform5, size_5);
    drawArrayBlocks(platform6, size_6);
    drawArrayBlocks(platform7, size_7);
    drawArrayBlocks(platform8, size_8);
    drawArrayBlocks(platform9, size_9);
    drawArrayBlocks(platform10, size_10);
    drawArrayBlocks(platform11, size_11);
    drawArrayBlocks(platform12, size_12);
    drawArrayBlocks(platform13, size_13);
    drawArrayBlocks(platform14, size_14);
    drawArrayBlocks(platform15, size_15);

    draw_vines(vines1, vines1_size);
    draw_vines(vines2, vines2_size);
    draw_vines(vines3, vines3_size);
    draw_vines(vines4, vines4_size);
    draw_vines(vines5, vines5_size);
    draw_vines(vines6, vines6_size);
    draw_vines(vines7, vines7_size);
    draw_vines(vines8, vines8_size);
    draw_vines(vines9, vines9_size);
    draw_vines(vines10, vines10_size);
    draw_vines(vines11, vines11_size);
    draw_vines(vines12, vines12_size);
    draw_vines(vines13, vines13_size);
    draw_vines(vines14, vines14_size);
    draw_vines(vines15, vines15_size);
    drawStaticImage(goal.x, goal.y, GOAL);
    // BOOLEAN FOR TOKEN SPAWN
    int token_spawned = 0;

    // GET CURRENT TIME FROM CLO BEFORE LEVEL LOOP STARTS
    time_t start_time;
    time(&start_time);

    int can_climb = 0;
    int colliding_top = 0;
    int colliding_bottom = 0;
    int colliding_left = 0;
    int colliding_right = 0;

    while (1)
    {
        // DECREMENT TIMER
        time(&state.clock);
        if (state.clock >= state.time_check)
        {
            --state.time;
            state.time_check += 1;
        }

        // SPAWN TOKEN IF 30 SECONDS HAS PASSED
        time(&state.clock);
        if (state.clock - start_time > 10 && !token_spawned)
        {
            drawImage(token.x, token.y, DK_TOKEN);
            draw_banana(&banana1);
            //  draw_banana(&banana4);
            // draw_banana(&banana5);

            // GOALPOST
            drawImage(clock.x, clock.y, clock.img_id);
            token_spawned = 1;
        }

        // CHECK FOR COLLISIONS WITH OBJECT AND ENTITIES

        if (token_spawned)
        {
            if (collide_token(&player, &token) && token.spawned)
            {
                ++state.lives;
				if (state.lives >= 999){
					state.lives = 999;
				}
            }
        }

        if (collide_banana(&player, &banana1) && token.spawned)
        {
            state.score += 10;
			if (state.score >= 999){
					state.score = 999;
			}
        }

        // for (int i = 0; i < banana_count; ++i)
        //{

        //}

        // CHECK PLAYER STATUS
        if (player_reached_goal(&player, &goal))
        {
            state.current_level_start_score = state.score;
            state.current_level_start_time = state.time;
            return 1;
        }

        /***
         * LOTS OF COLLISION DETECTIONS FOLLOW
         * 
         * 
        */
        if (player_take_damage(&player, &spider1, number_enemies))
        {
            --state.lives;
            return 0;
        }
        if (player_take_damage(&player, &spider2, number_enemies))
        {
            --state.lives;
            return 0;
        }
        if (player_take_damage(&player, &spider3, number_enemies))
        {
            --state.lives;
            return 0;
        }
        if (player_take_damage(&player, &spider4, number_enemies))
        {
            --state.lives;
            return 0;
        }
        if (player_take_damage(&player, &spider5, number_enemies))
        {
            --state.lives;
            return 0;
        }
        if (player_take_damage(&player, &spider6, number_enemies))
        {
            --state.lives;
            return 0;
        }
        if (player_take_damage(&player, &spider7, number_enemies))
        {
            --state.lives;
            return 0;
        }

        if (collide_clock(&player, &clock) && token.spawned)
        {
            state.time += 10;
			if (state.time >= 999){
				state.time = 999;
			}
        }
        if (player_fell(&player))
        {
            --state.lives;
            return 0;
        }


        // UPDATE TEXT AT TOP
        draw_lives(state.lives);
        draw_time(state.time);
        draw_score(state.score);




        // CHECCK FOR GRAVITY COLLISION WITH ANY PLATFORM FOR PLAYER (CAN BE POLISHED)
        for (int i = 0; i < number_platforms; ++i)
        {
            for (int j = 0; j < allPlatforms[i][0].array_size; ++j)
            {
                if (block_colliding_left(&player, &allPlatforms[i][j]))
                {
                    player.canMoveRight = 0;
                }
                if (block_colliding_right(&player, &allPlatforms[i][j]))
                {
                    player.canMoveLeft = 0;
                }
                if (block_colliding_bottom(&player, &allPlatforms[i][j]))
                {
                    player.canMoveUp = 0;
                    player.isJumping = 0;
                }

                // ADDITIONAL CHECK WHEN SEEING IF PLAYER IS COLLIDING WITH THE TOP OF A BLOCK (STANDING ON IT)

                // WHEN COLLIDING WITH A WALL, CHECKING COLLISION WITH THE TOP OF A BLOCK WILL MAKE THE PLAYER CLING
                // TO THE WALL. SO, IF WE ARE TOUCHING THE LEFT OR RIGHT SIDE OF A WALL/PLATFORM, WE CAN NEVER
                // BE COLLIDING WITH THE TOP OF IT, SO THE PLAYER WILL FALL
                // VERY IMPORTANT TO INCLUDE THE CONJUNCTION OF PLAYER BEING ABLE
                // TO MOVE LEFT AND RIGHT HERE
                if (block_colliding_top(&player, &allPlatforms[i][j]) && (player.canMoveLeft && player.canMoveRight))
                {
                    // player.playerCollidingTopBlock = 1;
                    player.canMoveDown = 0;
                }
            }
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                SDL_Scancode code = event.key.keysym.scancode;
                // check for vine climbing
                for (int i = 0; i < number_vines; ++i)
                {
                    for (int j = 0; j < allVines[i][0].array_size; ++j)
                    {
                        if (collide_vine(&player, &allVines[i][j], code))
                        {
                            player.canClimbUp = 1;
                            player.canClimbDown = 1;
                            player.canMoveDown = 0;
                            player.canJump = 0;
                        }
                    }
                }


            
                if (code == SDL_SCANCODE_ESCAPE && !state.paused)
                {
                    // run pause menu function, if result is not 0 exit level with fail code 0
                    int pause_result = pause_menu(wind);
                    if (pause_result == 1)
                    {
                        return 0;
                    }
                    else if (pause_result == -1)
                    {
                        return -1;
                    }
                    time(&state.time_check);
                }
                else if (code == SDL_SCANCODE_SPACE) {
                    player.doJumpLoop = 1;
                }
            

            // UPDATE PLAYER POSITION AND REDRAW PLAYER
            move_player(&player, code);
            }
        }
        gravity(&player);
        if (player.doJumpLoop)
            jump_player(&player);

        // move spiders
        climb_spider(&spider1);
        climb_spider(&spider2);
        climb_spider(&spider3);
        climb_spider(&spider4);
        climb_spider(&spider5);
        climb_spider(&spider6);
        climb_spider(&spider7);
    
        // reset collision detectors
        player.canMoveDown = 1;
        player.canClimbUp = 0;
        player.canClimbDown = 0;
        player.canMoveLeft = 1;
        player.canMoveRight = 1;
        player.doJumpLoop = 0;
        player.playerCollidingTopBlock = 0;
        colliding_top = 0;
        player.canJump = 1;
        SDL_UpdateWindowSurface(wind);
        SDL_Delay(1000 / 60);
    }
    return 1;
}

/**
 *  MAIN GAME FUNCTION AND OUTER GAME LOOP
 * 
*/
int main()
{

    /* START SDL*/

    /* Initializes the timer, audio, video, joystick,
     haptic, gamecontroller and events subsystems */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    /* Create a window */
    SDL_Window *wind = SDL_CreateWindow("Konkey's Dong",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        P_WIDTH, P_HEIGHT, 0);
    if (!wind)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(wind);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    if (!surface)
    {
        printf("Error creating surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }

    // record if game has been restarted
    state.restarted = 0;

    // Initialize GPIO controller, UART and framebuffer
    init_framebuffer(surface);

    // level complete indicators
    int level_one_done;
    int level_two_done;
    int level_three_done;
    int level_four_done;

    // main game loop
    while (1) {

        // set all levels to incomplete
        level_one_done = 0;
        level_two_done = 0;
        level_three_done = 0;
        level_four_done = 0;

        title_screen(wind);


        // if game not exited in title_screen()
        // Initialize the game state
        init_game_state(&state);
		
    
    // start the level 1 process
    while (level_one_done == 0)
    {       
            // clear screen then draw bar and levels
            quickClear();
            draw_bar();

            // get returned result from level 1's level loop / function
            level_one_done = level_1(wind);
    }

    // reset the backup frame buffer
    init_backup();

    // if player exited the game, go back to main menu
    if (level_one_done == -1)
    {
            continue;
    }

    // if player lost all levels or time, game over and start game loop agaian
    if (level_one_done == -2)
    {
            game_over(wind);
            continue;
    }
    

    /**
     * COMMENTS ABOVE FOR LEVEL 1 APPLY TO ALL OTHER LEVELS
     * 
     * 
    */
     while (level_two_done == 0)
     {
        quickClear();
        draw_bar();

        level_two_done = level_2(wind);
     }
     init_backup();
     if (level_two_done == -1)
     {
        continue;
     }
     if (level_two_done == -2) {
        game_over(wind);
        continue;
     }
     
    
    
     while (level_three_done == 0)
     {
        quickClear();
        draw_bar();

        level_three_done = level_3(wind);
     }
     init_backup();
     if (level_three_done == -1)
     {
        continue;
     }
     if (level_three_done == -2)
     {
         game_over(wind);
         continue;
     }
     
     
     
     
     while (level_four_done == 0)
     {
        quickClear();
        draw_bar();

        level_four_done = level_4(wind);
     }
     init_backup();
     if (level_four_done == -1)
     {
        continue;
     }
     if (level_four_done == -2)
     {
         game_over(wind);
         continue;
     }
     
     
    // if beat all levels, show win game and handle player and go back
    // to main screen if player clicks a button
     win_game(wind);
     SDL_Delay(1000 / 60);
    }
}


