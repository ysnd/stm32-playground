#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdlib.h>

#define LCD_ADDR (0x27 << 1)

I2C_HandleTypeDef hi2c1;

volatile uint32_t counter = 0;
uint32_t last_press1 = 0;
uint32_t last_press2 = 0;

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

void lcd_send_cmd(char cmd) {
    uint8_t data_u, data_l;
    uint8_t data_t [4];

    data_u = cmd & 0xF0;
    data_l = (cmd << 4) & 0xF0;

    data_t[0] = data_u | 0x0C;
    data_t[1] = data_u | 0x08;
    data_t[2] = data_l | 0x0C;
    data_t[3] = data_l | 0x08;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

void lcd_send_data(char data) {
    uint8_t data_u, data_l;
    uint8_t data_t[4];

    data_u = data & 0xF0;
    data_l = (data << 4) & 0xF0;

    data_t[0] = data_u | 0x0D;
    data_t[1] = data_u | 0x09;
    data_t[2] = data_l | 0x0D;
    data_t[3] = data_l | 0x09;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_set_cursor(int row, int col) {
    if (row==0) {
        lcd_send_cmd(0x80 + col);
    }
    if (row==1) {
        lcd_send_cmd(0xC0 + col);
    }
}

void lcd_print(char *str) {
    while (*str) lcd_send_data(*str++);    
}

void lcd_clear_line(int line) {
    lcd_set_cursor(line, 0);
    for (int i=0; i<16; i++) {
        lcd_send_data(' ');
    }
    lcd_set_cursor(line, 0);
}

void lcd_init(void) {
    HAL_Delay(50);
    
    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x30);

    lcd_send_cmd(0x20);

    lcd_send_cmd(0x28);
    lcd_send_cmd(0x08);
    lcd_send_cmd(0x01);
    HAL_Delay(2);
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x0C);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        if (HAL_GetTick() - last_press1 > 200) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
            counter++;
            last_press1 = HAL_GetTick();
        }
    }
    else if (GPIO_Pin == GPIO_PIN_1) {
        if (HAL_GetTick() - last_press2 > 200) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            counter--;
            last_press2 = HAL_GetTick();
        }
    }
}

void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

int main(void)
{  
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();  
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    lcd_init();
    GPIO_InitTypeDef GPIO_InitStruct={0};

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    char buf[20];

    while (1) {
    lcd_set_cursor(0, 0);
    lcd_print("CounterToggleLED");
    lcd_clear_line(1);
    lcd_set_cursor(1, 0);
    lcd_print("Count: ");
    itoa((int)counter, buf, 10);
    lcd_set_cursor(1, 7);
    lcd_print(buf);
    HAL_Delay(100);
    }
}


