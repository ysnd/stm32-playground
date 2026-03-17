#include "stm32f103xb.h"

void clock_init() {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}

void gpio_init() {
    //enable clock 
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    //pb13 output
    GPIOB->CRH &= ~(0xF << ((13 - 8)*4));
    GPIOB->CRH |= (0x2 << ((13 - 8)*4));

    //pa0 input pull-up
    GPIOA->CRL &= ~(0xF << (0*4));
    GPIOA->CRL |= (0x8 << (0*4));
    GPIOA->ODR |= ( 1 << 0);

    //exti line 0 = pa0
    AFIO->EXTICR[0] &= ~(0xF << 0); //map exti0 ke pa0

    //trigg falling edge
    EXTI->IMR |= (1 << 0); //unmask
    EXTI->FTSR |= (1 << 0); //falling trigger

    //enable intterupt di nvic
    NVIC_EnableIRQ(EXTI0_IRQn);
}

//intterupt handler
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1<<0)) {
        GPIOB->ODR ^= (1 << 13);

        EXTI->PR |= (1 << 0);
    }
}

int main() {
    clock_init();
    gpio_init();
    while (1) {
    
    }
}
