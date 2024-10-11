#include "framebuffer.h"
#include "image.h"
#include "text.h"

/**
 * Class for drawing test to screen (numbers)
 * 
 * 
*/


/**
 * Function for drawing a number to screen
 * 
 * 
*/
void draw_int(int x, int y, int num) {
    // set array of max number of digits
    int buffer[DIGIT_MAX];
    int offset = DIGIT_MAX - 1;

    // if 0, just draw image 0
    if (num == 0) {
        drawImage(x, y, NUMBER_0);
        return;
    }
    
    // otherwise, set each value to -1 in array
    for (int i = 0; i < DIGIT_MAX; ++i) {
        buffer[i] = -1;
    }

    // iterate from right to left of array, adding each digit
    // to array in order

    // ex: 123 ->    {-1,-1,-,1, 1, 2, 3};
    int rolling = num;
    while (rolling >= 1) {
        int digit = rolling % 10;
        rolling = rolling / 10;
        buffer[offset] = digit;
        --offset;
    }

    // iterate over array and draw each image if it is not -1
    int digit_count = 0;
    for (int i = 0; i < DIGIT_MAX; ++i) {
        if (buffer[i] != - 1) {
            drawImage(x + (digit_count * TEXT_WIDTH), y, NUMBER_0 + buffer[i]);
            ++digit_count;
        }
        else {
            drawImage(x + (digit_count * TEXT_WIDTH), y, NUMBER_0);
            ++digit_count;
        }
    }
}

/**
 * Draw all black over a letter to clear it
 * 
*/
void clear_int(int x, int y) {
        int buffer[DIGIT_MAX];
        int digit_count = 0;
        for (int i = 0; i < DIGIT_MAX; ++i) {
        if (buffer[i] != - 1) {
            drawRect(x + (digit_count * TEXT_WIDTH), y, x + (digit_count* TEXT_WIDTH) + TEXT_WIDTH, TEXT_HEIGHT, 0x00000000, 1);
            ++digit_count;
        }
    }
}

/*
    Draw black bar at top of screen with word test
*/
void draw_bar() {
    drawRect(0, 0, P_WIDTH - 1, TEXT_HEIGHT, 0x00000000, 1);
    drawImage(TEXT_0_LIVES, 0, TEXT_LIVES);
    drawImage(TEXT_0_TIME, 0, TEXT_TIME);
    drawImage(TEXT_0_SCORE, 0, TEXT_SCORE);
}

/**
 * Draw number lives
 * 
*/
void draw_lives(int num) {
    draw_int(NUMBER_0_LIVES, 0, num);
}

// time
void draw_time(int num) {
    draw_int(NUMBER_0_TIME, 0, num);
}

// score
void draw_score(int num) {
    draw_int(NUMBER_0_SCORE, 0, num);
}