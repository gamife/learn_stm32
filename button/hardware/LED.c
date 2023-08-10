#include "stm32f10x.h"

void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_All,
    };

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_All);
}

/**
  * @brief  反转GPIOA的pin脚的电平
  * @param  GPIO_Pin: specifies the port bit to be written.
  *   This parameter can be one of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void LED_Turn(uint16_t GPIO_Pin)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin,  (BitAction)(1 ^ GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin)));
}

void LED_ON(uint16_t pin)
{
    GPIO_ResetBits(GPIOA, pin);
}

void LED_OFF(uint16_t pin)
{
    GPIO_SetBits(GPIOA, pin);
}

void LED1_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void LED1_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED2_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void LED2_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_2);
}
