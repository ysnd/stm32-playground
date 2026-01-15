#include "stm32f1xx_hal.h"
#include <stdint.h>

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint8_t last;
    uint8_t stable;
    uint8_t t_start;
} Button;

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

void Button_Update(Button* b) {
    uint8_t now = HAL_GPIO_ReadPin(b->port, b->pin);

    if (now != b->last) {
        b->t_start = HAL_GetTick();
        b->last = now;
    }
    if ((HAL_GetTick() - b->t_start) >10) {
        b->stable = b->last;
    }
}


int main(void)
{
    HAL_Init();
    SystemClock_Config();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);

    Button btn0;
    Button btn1;

    btn0.port = GPIOA;
    btn0.pin = GPIO_PIN_0;
    btn0.stable = 1;
    btn0.last = 1;
    btn0.t_start = 0;

    btn1.port = GPIOA;
    btn1.pin = GPIO_PIN_1;
    btn1.stable = 1;
    btn1.last = 1;
    btn1.t_start = 0;


    while (1)
    {
	    Button_Update(&btn0);
        Button_Update(&btn1);
        if (btn0.stable == 0) {
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
		} else {
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
        }

        if (btn1.stable == 0) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
        } else {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
        }
    }
}


