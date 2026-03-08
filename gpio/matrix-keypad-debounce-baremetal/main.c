#include "stm32f103xb.h"
#include <stdint.h>

#define NUM_ROWS 4
#define NUM_COLS 4
#define DEBOUNCE_MS 20

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

uint32_t millis(void) {
    return ms_ticks;
}
    
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} PinDef;

typedef struct {
    PinDef row;
    uint8_t state;
    uint8_t last;
    uint32_t t_start;
} RowDebounce;

PinDef cols[NUM_COLS] = {
    {GPIOB, GPIO_PIN_0},
    {GPIOB, GPIO_PIN_1},
    {GPIOB, GPIO_PIN_3},
    {GPIOB, GPIO_PIN_4}
};

RowDebounce rows[NUM_ROWS] = {
    {{GPIOA, GPIO_PIN_0}, 1, 1, 0}
    {{GPIOA, GPIO_PIN_1}, 1, 1, 0}
    {{GPIOA, GPIO_PIN_2}, 1, 1, 0}
    {{GPIOA, GPIO_PIN_3}, 1, 1, 0}
};

char keymap[NUM_ROWS][NUM_COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void GPIO_init(void) {
    RCC->APB2ENR |= (1<<2) | (1<<3);

    for (int i=0; i<NUM_ROWS; i++) {
        GPIO_TypeDef* port = rows[i].row.port;
        uint16_t pin = rows[i].row.pin;
        if (pin < 8) { //CRL
        
        }
    }




int main(void) {
    SysTick_Config(SystemCoreClock / 1000);


    while (1) {
        uint8_t now;
        if (GPIOA->IDR & (1 << 0)) { 
            now = 1;
        } else {
            now = 0;
        }
        if (now != last) {
            t_start = ms_ticks;
            last = now;
        }
        if ((ms_ticks - t_start) > 10) {
            stable = last;
        }
        if (stable == 0) {
            GPIOB->BSRR = (1 << 13); //on 
        } else {                
            GPIOB->BSRR = (1 << (13 + 16)); //off   
        }
    }
}
