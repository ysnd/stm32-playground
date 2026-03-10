#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <string.h>

#define LCD_ADDR (0x27 << 1)

I2C_HandleTypeDef hi2c1;

char keymap[4][4] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

int cursor_pos = 0;

/* ================= LCD LOW LEVEL ================= */

void lcd_send_cmd(char cmd)
{
uint8_t data_u, data_l;
uint8_t data_t[4];

data_u = cmd & 0xF0;
data_l = (cmd << 4) & 0xF0;

data_t[0] = data_u | 0x0C;
data_t[1] = data_u | 0x08;
data_t[2] = data_l | 0x0C;
data_t[3] = data_l | 0x08;

HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

void lcd_send_data(char data)
{
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

/* ================= LCD UTILITY ================= */

void lcd_clear()
{
lcd_send_cmd(0x01);
HAL_Delay(2);
}

void lcd_set_cursor(int row, int col)
{
if(row==0)
lcd_send_cmd(0x80 + col);

if(row==1)
lcd_send_cmd(0xC0 + col);
}

void lcd_print(char *str)
{
while(*str) lcd_send_data(*str++);
}

void lcd_clear_line(int line)
{
lcd_set_cursor(line,0);

for(int i=0;i<16;i++)
lcd_send_data(' ');

lcd_set_cursor(line,0);
}

/* ================= LCD INIT ================= */

void lcd_init()
{
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

/* ================= KEYPAD SCAN ================= */

char keypad_scan()
{
for(int row=0; row<4; row++)
{

HAL_GPIO_WritePin(GPIOA,
GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
GPIO_PIN_SET);

HAL_GPIO_WritePin(GPIOA,(GPIO_PIN_0<<row),GPIO_PIN_RESET);

for(int col=0; col<4; col++)
{

if(HAL_GPIO_ReadPin(GPIOB,(GPIO_PIN_4<<col))==GPIO_PIN_RESET)
{

HAL_Delay(20);

if(HAL_GPIO_ReadPin(GPIOB,(GPIO_PIN_4<<col))==GPIO_PIN_RESET)
{

while(HAL_GPIO_ReadPin(GPIOB,(GPIO_PIN_4<<col))==GPIO_PIN_RESET);

return keymap[row][col];

}

}

}

}

return 0;
}

/* ================= CLOCK ================= */

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

HAL_RCC_OscConfig(&RCC_OscInitStruct);

RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK
| RCC_CLOCKTYPE_SYSCLK
| RCC_CLOCKTYPE_PCLK1
| RCC_CLOCKTYPE_PCLK2;

RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

/* ================= I2C ================= */

static void MX_I2C1_Init(void)
{

hi2c1.Instance = I2C1;

hi2c1.Init.ClockSpeed = 100000;
hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;

hi2c1.Init.OwnAddress1 = 0;

hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;

hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;

hi2c1.Init.OwnAddress2 = 0;

hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;

hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

HAL_I2C_Init(&hi2c1);

}

/* ================= MAIN ================= */

int main(void)
{

HAL_Init();
SystemClock_Config();

__HAL_RCC_GPIOA_CLK_ENABLE();
__HAL_RCC_GPIOB_CLK_ENABLE();

MX_I2C1_Init();

GPIO_InitTypeDef GPIO_InitStruct={0};

/* ROW OUTPUT */

GPIO_InitStruct.Pin =
GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;

GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

/* COLUMN INPUT */

GPIO_InitStruct.Pin =
GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;

GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;

HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

lcd_init();

lcd_set_cursor(0,0);
lcd_print("KEYPAD TEST");

lcd_set_cursor(1,0);

while(1)
{

char key = keypad_scan();

if(key)
{

lcd_set_cursor(1,cursor_pos);
lcd_send_data(key);

cursor_pos++;

if(cursor_pos>=16)
{
lcd_clear_line(1);
cursor_pos=0;
}

}

}

}
