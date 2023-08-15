#include "stm32f10x.h"
#include "Encoder.h"

int32_t Encoder_Count = 0;

void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_IPU,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,   GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,   GPIO_PinSource1);

    EXTI_InitTypeDef EXTI_InitStructure = {
        .EXTI_Line = EXTI_Line0 | EXTI_Line1,
        .EXTI_Mode = EXTI_Mode_Interrupt,
        .EXTI_Trigger = EXTI_Trigger_Falling,
        .EXTI_LineCmd = ENABLE,
    };
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = EXTI0_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    };
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

int32_t Encoder_Get(void){
    int32_t temp;
    temp = Encoder_Count;
    Encoder_Count = 0;
    return temp;
}

// EXTI0中断类型的中断处理函数
void EXTI0_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line0) == SET){
        // 旋转编码器, 当A出现下降沿, 检查B是否是0, 是0则旋转了
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0){
            Encoder_Count --;
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

// EXTI0中断类型的中断处理函数
void EXTI1_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line1) == SET){
            // 旋转编码器, 当B出现下降沿, 检查A是否是0, 是0则旋转了
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0){
            Encoder_Count ++;
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}
