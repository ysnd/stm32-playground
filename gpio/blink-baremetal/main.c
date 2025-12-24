
#include "stm32f1xx.h"

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}

int main(void) {
    // SysTick 1ms tick
    SysTick_Config(SystemCoreClock / 1000);

    // enable clock GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // configure PB13 output push-pull, 2MHz
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_1;

    while (1) {
        GPIOC->BRR = (1 << 13); // ON
        delay_ms(500);
        GPIOC->BSRR = (1 << 13); // OFF
        delay_ms(500);
    }
}

