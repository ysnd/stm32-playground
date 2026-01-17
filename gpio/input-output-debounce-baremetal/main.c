#include "stm32f103xb.h"
#include <stdint.h>
volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

int main(void) {
    SysTick_Config(SystemCoreClock / 1000);
    // enable clock gpioa gpiob
    RCC->APB2ENR |= (1 << 2);
    RCC->APB2ENR |= (1 << 3);

    // pa0 input pull-up
    GPIOA->CRL &= ~(0xF << (0 * 4));
    GPIOA->CRL |= (0x8 << (0 * 4)); // input pullup pulldown
    GPIOA->ODR |= (1 << 0); // pull-up

    //pb13 output push-pull
    GPIOB->CRH &= ~(0xF << ((13 - 8) * 4));
    GPIOB->CRH |= (0x2 << ((13 - 8) * 4));

    uint8_t last = 1;
    uint8_t stable = 1;
    uint32_t t_start = 0;

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
