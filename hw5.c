#include <stdint.h>
#include <stdbool.h>
#include "baremetal_binary.h" 

#define MAX_TICK (60)

#define VIEW_LIGHT  ((volatile uint8_t * const)0xE020U)  // 0xE020~0xE02F
#define VIEW_SCORE  ((volatile uint8_t * const)0xE800U)   

#define RANDOM_DISP ((volatile uint8_t * const)0xE808U)  

#define CONTROLLER  ((volatile const uint8_t * const)0xD010U)  

uint8_t random4(uint8_t seed){
    static const uint8_t seq4[256] = {
        0x3, 0xE, 0x1, 0x9, 0xC, 0x5, 0xB, 0x7, 0x2, 0xF, 0x8, 0x0, 0xA, 0x6, 0x4, 0xD,
        0x9, 0x2, 0xF, 0xB, 0x4, 0x7, 0x0, 0xE, 0xC, 0x5, 0x8, 0xA, 0x1, 0x6, 0xD, 0x3,
        0xE, 0x8, 0x5, 0x0, 0xB, 0x3, 0x9, 0x6, 0xF, 0x2, 0xC, 0xA, 0x7, 0x4, 0xD, 0x1,
        0x6, 0xB, 0x2, 0xD, 0x8, 0x1, 0xE, 0x5, 0x0, 0x9, 0x4, 0xF, 0xA, 0x7, 0xC, 0x3,
        0xA, 0x4, 0xD, 0x8, 0x1, 0xE, 0x5, 0x2, 0xF, 0x9, 0xC, 0x3, 0x7, 0xB, 0x0, 0x6,
        0x5, 0xC, 0x7, 0x1, 0xE, 0x8, 0xB, 0x4, 0x9, 0x2, 0xF, 0x0, 0xD, 0x6, 0xA, 0x3,
        0xF, 0x0, 0x9, 0x3, 0xA, 0x6, 0xD, 0x8, 0x2, 0xB, 0x5, 0xE, 0x1, 0xC, 0x7, 0x4,
        0x1, 0x7, 0xC, 0xA, 0x3, 0xF, 0x6, 0x9, 0xD, 0x4, 0x0, 0x2, 0x8, 0xE, 0xB, 0x5,
        0x8, 0xD, 0x4, 0xF, 0x2, 0x9, 0x0, 0xC, 0x6, 0xA, 0x3, 0xB, 0xE, 0x5, 0x1, 0x7,
        0xC, 0x3, 0xA, 0x6, 0xF, 0x0, 0xD, 0x5, 0x8, 0x1, 0xE, 0x9, 0x4, 0x7, 0x2, 0xB,
        0x2, 0x9, 0xE, 0x4, 0x7, 0xB, 0x1, 0xA, 0x5, 0xD, 0x8, 0x6, 0xC, 0x0, 0xF, 0x3,
        0xD, 0x6, 0x8, 0x2, 0xB, 0x4, 0x9, 0xE, 0x3, 0xF, 0x1, 0x7, 0xA, 0xC, 0x5, 0x0,
        0x7, 0x1, 0x5, 0xC, 0x9, 0xD, 0x2, 0xF, 0xB, 0x8, 0xE, 0x3, 0x6, 0xA, 0x4, 0x0,
        0x4, 0xA, 0x3, 0x7, 0x6, 0x0, 0xF, 0xB, 0x1, 0xE, 0x9, 0xD, 0x2, 0x5, 0x8, 0xC,
        0xB, 0x5, 0x0, 0xE, 0xD, 0x7, 0xA, 0x1, 0x4, 0xC, 0x2, 0x8, 0xF, 0x3, 0x6, 0x9,
        0x0, 0xF, 0x6, 0xB, 0x5, 0x2, 0x8, 0xC, 0x7, 0x3, 0xA, 0x4, 0x9, 0x1, 0xD, 0xE
    };
    static uint8_t counter = 7;
    counter = counter + seed + 1;
    return seq4[counter];
}

typedef struct {
    uint8_t player_col; // Player is always Row 0
    uint8_t coin_row;
    uint8_t coin_col;
    uint8_t score;      // Range 0-99
    uint8_t tick;       // Speed control (Time State)
    //uint8_t matrix[16];
} model_t;

typedef enum {
    NONE, LEFT, RIGHT, RESTART
} command;

void view_reset(void)
{
    for (uint8_t i = 0; i < 16; i++) {
        *(VIEW_LIGHT + i) = B8(00000000);
    }
    *VIEW_SCORE = 0;
}

void model_init(model_t *mp, uint8_t random4)
{  
    /*
    for (uint8_t i = 0; i < 16; i++) {
        mp->matrix[i] = B8(00000000);
    }
    */
    mp->player_col = 7;
    mp->coin_row = 7;
    mp->coin_col = random4 % 16;
    mp->score = 0;
    mp->tick = 0;
}

void model_update(model_t *mp, uint8_t random4, command c)
{
    switch(c)
    {
        case LEFT: 
            if (mp->player_col != 0) 
            {
                mp->player_col --;
            }
            break;
        case RIGHT:
            if (mp->player_col != 15) 
            {
                mp->player_col ++;
            }
            break;
        case RESTART:
            model_init(mp, random4);
            view_reset();
            break;
        case NONE: 
            break;
    }

    if (mp->coin_row == 0 && mp->coin_col == mp->player_col)
        {
            if(mp->score <= 96) mp->score += 3;
            mp->coin_row = 7;
            mp->coin_col = random4 % 16;
        }
    else
    {
        mp->tick ++;
        if (mp->tick == MAX_TICK) 
        {
            mp->tick = 0;
            if (mp->coin_row > 0) mp->coin_row --;
            else if(mp->coin_row == 0)
            {
                mp->coin_row = 7;
                if(mp->score > 0) mp->score --;
            }
        }
    }
}

uint8_t change(uint8_t a)
{
    uint8_t value = a;
    uint8_t ten = value / 10;
    uint8_t unit = value % 10;
    uint8_t display_value = (ten << 4) | unit;  // 高4位存十位，低4位存个位
    return display_value;                       // 把value十位存储在地址前四位，个位存储在地址后四位
}

void view_update(const model_t *mp){
    static uint8_t previous_coin_row = 0;
    static uint8_t previous_coin_col = 0;
    static uint8_t previous_player_col = 0;
    static uint8_t previous_score = 0;
    if( mp->player_col == previous_player_col &&
        mp->coin_col == previous_coin_col &&
        mp->coin_row == previous_coin_row &&
        mp->score == previous_score){
            return;
        }
    else 
    {
        *VIEW_SCORE = change(mp->score);
        *(VIEW_LIGHT + previous_player_col) = B8(00000000);
        if(mp->coin_col == mp->player_col)
        {
            *(VIEW_LIGHT + mp->player_col) = B8(00000001) | (1 << mp->coin_row);
        }
        else{
            *(VIEW_LIGHT + mp->player_col) = B8(00000001);
            *(VIEW_LIGHT + mp->coin_col) = (1 << mp->coin_row);
        }
        previous_coin_col = mp->coin_col;
        previous_coin_row = mp->coin_row;
        previous_player_col = mp->player_col;
        previous_score = mp->score;
    }
    
}

command controller_read(void)
{  
    uint8_t key = *CONTROLLER;
    switch (key)
    {
        case B8(10100000): return LEFT;
        case B8(10100010): return RIGHT;
        case B8(10000010): return RESTART;
        default: return NONE;
    }
}

void main(void){
    model_t m;
    model_t *mp = &m;
    command c;
    uint8_t random = 0;
    model_init(mp, random);
    while (true){
        c = controller_read();
        model_update(mp, random, c);
        view_update(mp);
        *RANDOM_DISP = random;
        random = random + 1 + (uint8_t ) c;
    }
}
