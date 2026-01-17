#include "stm32f103xb.h"
#include <stdint.h>

void delay(volatile uint32_t t) {
    while (t--);
}

int main(void) {
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

    while (1) {
        if (!(GPIOA->IDR & (1 << 0))) { //button pressed
            delay(30000);
            if (!(GPIOA->IDR & (1 << 0))) {
                GPIOB->ODR |= (1 << 13); //on 
            } else {
                GPIOB->ODR &= ~(1 << 13); //off   
            }
        }
    }
}
