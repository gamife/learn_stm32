#include "stm32f10x.h"
#include "Delay.h"
#include "Key.h"

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        // 上拉输入模式
        .GPIO_Mode = GPIO_Mode_IPU,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12,
    };

    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

KEY_NUM Key_GetNum(void)
{
    KEY_NUM key_num = KEY_NO;

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET){
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET);
        Delay_ms(20);
        key_num = KEY1;
    }
    
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == Bit_RESET){
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == Bit_RESET);
        Delay_ms(20);
        key_num = KEY2;
    }

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == Bit_RESET){
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == Bit_RESET);
        Delay_ms(20);
        key_num = KEY3;
    }
    return key_num;
}
