#include "stm32f10x.h"

// 蜂鸣器初始化
void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_12,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_Turn(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12,  (BitAction)(1 ^ GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12)));
}

void Buzzer_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}
