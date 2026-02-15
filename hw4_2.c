#include <stdint.h>
#include <stdbool.h>

// VIEW
#define VIEW_PERIOD ((volatile uint8_t * const)0xE800U)
#define VIEW_LIGHT  ((volatile bool    * const)0xE000U)

// CONTROLLER
#define CONTROLLER_PLUS  ((volatile const bool * const)0xD007U)
#define CONTROLLER_MINUS ((volatile const bool * const)0xD006U)

// MODEL
typedef struct {
    bool light;
    uint8_t period;
    uint8_t count;
} model_t;

typedef enum {
    NONE, PLUS, MINUS
} command;

void model_init(model_t *mp){  // 初始化所有状态
    mp->count = 0;
    mp->light = true;
    mp->period = 35;
}

void model_update(model_t *mp, command c){  // 判断按了哪一个按钮，应该加速/减速/不变
    switch (c) {
        case MINUS: if ( mp->period < 95) {
                        mp->period = mp->period + 5; 
                    }
                    break;
        case PLUS:  if (mp->period > 5 ) {
                        mp->period = mp->period - 5; 
                    }
                    break;
        case NONE:  break;
        default:    break;
    }
    mp->count = mp->count + 1;          // 每次调用这个函数，计数器加1,通过CPU频率，不断累加实现内部时钟
    if(mp->count == mp->period){        // 当计数器达到设定的周期值时，翻转灯的状态
        mp->light = !(mp->light);       // 翻转灯的状态
        mp->count = 0;                  // 计数器清零
    }
}

// VIEW
void view_update(const model_t *mp){   // 更新状态机的状态
    // update light
    *VIEW_LIGHT = mp->light;

    uint8_t tens = mp->period / 10;
    uint8_t units = mp->period % 10;
    uint8_t display_value = (tens << 4) | units; // 高4位存十位，低4位存个位
    *VIEW_PERIOD = display_value;
}

// CONTROLLER
command controller_read(void){  // 读取按钮状态
    if (*CONTROLLER_PLUS) {
        return PLUS;
    }
    if (*CONTROLLER_MINUS) {
        return MINUS;
    }else{
        return NONE;
    }
}

void main(void){
    model_t m;
    model_t *mp = &m;
    command c;
    model_init(mp);   // 初始化
    while (true) {
        // VIEW  UPDATE
        view_update(mp);
        // CONTROLLER
        c = controller_read();
        // MODEL UPDATE
        model_update(mp, c);
    }
}
