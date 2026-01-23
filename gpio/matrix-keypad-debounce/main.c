#include "stm32f1xx_hal.h"
#include <stdint.h>

#define NUM_ROWS 4
#define NUM_COLS 4
#define DEBOUNCE_MS 20

GPIO_TypeDef* row_ports[NUM_ROWS] = {GPIOA, GPIOA, GPIOA, GPIOA};
uint16_t row_pins[NUM_ROWS] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};

GPIO_TypeDef* col_ports[NUM_COLS] = {GPIOB, GPIOB, GPIOB, GPIOB};
uint16_t col_pins[NUM_COLS] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_3, GPIO_PIN_4};

typedef struct {
    uint8_t last;
    uint8_t state;
    uint8_t t_start;
} RowDebounce;

RowDebounce rows[NUM_ROWS] = {0};

char keymap[NUM_ROWS][NUM_COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

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

void GPIO_init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    for (int i=0; i<NUM_ROWS; i++) {
        GPIO_InitStruct.Pin = row_pins[i];
        HAL_GPIO_Init(row_ports[i], &GPIO_InitStruct);
    }

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    for (int i=0; i<NUM_COLS; i++) {
        GPIO_InitStruct.Pin = col_pins[i];
        HAL_GPIO_Init(col_ports[i], &GPIO_InitStruct);
        HAL_GPIO_WritePin(col_ports[i], col_pins[i], GPIO_PIN_SET);
    }
}

char scan_matrix(void) {
    for (int c=0;c<NUM_COLS;c++) {
        for (int i=0; i<NUM_COLS; i++) {
            HAL_GPIO_WritePin(col_ports[i], col_pins[i], (i==c)? GPIO_PIN_RESET : GPIO_PIN_SET);

        }

        for (int r=0;r<NUM_ROWS;r++) {
            uint8_t now = HAL_GPIO_ReadPin(row_ports[r], row_pins[r]);
            RowDebounce* row = &rows[r];
            if (now != row->last) {
                row->t_start = HAL_GetTick();
                row->last = now;
            }

            if ((HAL_GetTick() - row->t_start) > DEBOUNCE_MS) {
                if (row->state != now) {
                    row->state = now;
                    if (now == GPIO_PIN_RESET) {
                        return keymap[r][c];
                    }
                }
            }
        }
    }
    return 0;
}


int main(void)
{
    HAL_Init();
    SystemClock_Config();
    GPIO_init();

    while (1)
    {
	    char key = scan_matrix();
        if (key) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
            HAL_Delay(100);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
        }
    }
}


