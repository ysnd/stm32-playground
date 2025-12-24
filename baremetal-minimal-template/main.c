#include <stdint.h>

#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define GPIOC_CRH (*(volatile uint32_t*)0x40011004)
#define GPIOC_ODR (*(volatile uint32_t*)0x4001100C)

void delay(void) {
    for (volatile uint32_t i = 0; i < 500000; i++);
}

int main(void) {
    // Enable GPIOC clock
    RCC_APB2ENR |= (1 << 4);

    //pc13 output push pull
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |= (0x1 << 20);

    while (1) {
        GPIOC_ODR ^= (1 << 13);
        delay();
    }
}
