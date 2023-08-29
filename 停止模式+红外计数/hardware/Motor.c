#include "stm32f10x.h"
#include "PWM.h"
#include "Motor.h"

/**
  * @brief  电机初始化
  *   TIM2_CH3(PA2)控制速度, PA4和PA5控制转动方向
  * @param  None
  * @retval None
  */
void Motor_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5,
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);

    PWM_Motor_Init();
}

/**
  * @brief  设置电机转动方向和速度
  * @param  speed: 要设置的角度. 
  *     取值范围,-100~100 
  * @retval None
  */
void Motor_SetSpeed(int16_t speed){
    if (speed >= 0){
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        PWM_Motor_SetSpeed(speed);
    }else{
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        PWM_Motor_SetSpeed(-speed);
    }
}
