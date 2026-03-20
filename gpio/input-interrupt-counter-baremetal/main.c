#include "stm32f103xb.h"
#include <stdint.h>
#include <stdlib.h>

#define LCD_ADDR 0x27
volatile uint32_t ms_ticks = 0;

uint32_t last_press0 = 0;
uint32_t last_press1 = 0;
int32_t count = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}

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

void systick_init(void) {
    SysTick->LOAD = 72000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 7; //enable + interrupt + core clock 
}

void gpio_init() {
    //enable clock 
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    //pb13 output
    GPIOB->CRH &= ~(0xF << ((13 - 8)*4));
    GPIOB->CRH |= (0x2 << ((13 - 8)*4));

    //pc13 out 
    GPIOC->CRH &= ~(0xF << ((13 - 8)*4));
    GPIOC->CRH |= (0x2 << ((13 - 8)*4));

    GPIOB->CRL &= ~(0xF << (6*4));
    GPIOB->CRL |= (0xF << (6*4)); //af open drain

    //pb7 SDA
    GPIOB->CRL &= ~(0xF << (7*4));
    GPIOB->CRL |= (0xF << (7*4));

    //pa0 input pull-up
    GPIOA->CRL &= ~(0xF << (0*4));
    GPIOA->CRL |= (0x8 << (0*4));
    GPIOA->ODR |= (1 << 0);

    //pa1 input pullup 
    GPIOA->CRL &= ~(0xF << (1*4));
    GPIOA->CRL |= (0x8 << (1*4));
    GPIOA->ODR |= (1 << 1);

    //exti line 0 = pa0
    AFIO->EXTICR[0] &= ~(0xF << 0);//map exti0 ke pa0
    AFIO->EXTICR[0] &= ~(0xF << 4);//map exti1 ke pa1 

    //trigg falling edge pa0 pa1
    EXTI->IMR |= (1 << 0) | (1 << 1); //unmask
    EXTI->FTSR |= (1 << 0) | (1 << 1); //falling trigger

    //enable intterupt di nvic
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

//intterupt handler
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) {
        if ((ms_ticks - last_press0) > 200) {
            GPIOB->ODR ^= (1 << 13);
            count++;
            last_press0 = ms_ticks;
        }
        EXTI->PR |= (1 << 0);
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1 << 1)) {
        if ((ms_ticks - last_press1) > 200) {
            GPIOC->ODR ^= (1 << 13);
            count--;
            last_press1 = ms_ticks;
        }
        EXTI->PR |= (1 << 1);
    }
}

void i2c1_init() {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C1->CR1 &= ~I2C_CR1_PE;

    I2C1->CR2 = 36; //apb1 36Mhz
    I2C1->CCR = 180; //100khz
    I2C1->TRISE = 37;

    I2C1->CR1 |= I2C_CR1_PE;
}

void i2c_start() {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void i2c_stop() {
    I2C1->CR1 |= I2C_CR1_STOP;
}

void i2c_write(uint8_t data) {
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
}

void i2c_addr(uint8_t addr) {
    I2C1->DR = addr;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
}

void i2c_tx(uint8_t addr, uint8_t *data, int len) {
    i2c_start();
    i2c_addr(addr << 1);
    for (int i=0 ; i<len; i++) {
        i2c_write(data[i]);
    }

    while (!(I2C1->SR1 & I2C_SR1_BTF));
    i2c_stop();
}

void lcd_send_cmd(uint8_t cmd) {
    uint8_t data_u = cmd & 0xF0;
    uint8_t data_l = (cmd << 4) & 0xF0;

    uint8_t data_t[4];

    data_t[0] = data_u | 0x0C;
    data_t[1] = data_u | 0x08;
    data_t[2] = data_l | 0x0C;
    data_t[3] = data_l | 0x08;

    i2c_tx(LCD_ADDR, data_t, 4);
}

void lcd_send_data(char data) {
    uint8_t data_u = data & 0xF0;
    uint8_t data_l = (data << 4) & 0xF0;

    uint8_t data_t[4];

    data_t[0] = data_u | 0x0D;
    data_t[1] = data_u | 0x09;
    data_t[2] = data_l | 0x0D;
    data_t[3] = data_l | 0x09;

    i2c_tx(LCD_ADDR, data_t, 4);
}

void lcd_init() {
    delay_ms(50);
    lcd_send_cmd(0x30);
    delay_ms(5);

    lcd_send_cmd(0x30);
    delay_ms(1);

    lcd_send_cmd(0x30);

    lcd_send_cmd(0x20);

    lcd_send_cmd(0x28);
    lcd_send_cmd(0x08);
    lcd_send_cmd(0x01);

    delay_ms(2);

    lcd_send_cmd(0x06);
    lcd_send_cmd(0x0C);
}

void lcd_set_cursor(int row, int col) {
    if (row==0) {
        lcd_send_cmd(0x80 + col);
    } else {
        lcd_send_cmd(0xC0 + col);
    }
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_clear_line(int line) {
    lcd_set_cursor(line, 0);
    for (int i=0; i<16; i++) {
        lcd_send_data(' ');
    }
    lcd_set_cursor(line, 0);
}

void lcd_clear() {
    lcd_send_cmd(0x01);
    delay_ms(2);
}

int main() {
    clock_init();
    systick_init();
    gpio_init();
    i2c1_init();
    lcd_init();

    while (1) {
        char buf[20];
        lcd_set_cursor(0, 0);
        lcd_print("Menghitung......");
        lcd_clear_line(1);
        lcd_set_cursor(1, 0);
        lcd_print("Total: ");
        itoa((int)count, buf, 10);
        lcd_set_cursor(1, 8);
        lcd_print(buf);
        delay_ms(100);
    }
}

