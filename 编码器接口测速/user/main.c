#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "LightSensor.h"
#include "OLED.h"
#include "CountSensor.h"
#include "Encoder.h"
#include "Timer.h"
#include "PWM.h"
#include "Servo.h"
#include "Motor.h"
#include "IC.h"
#include "EncoderTIM.h"

int32_t Speed;

int main(void)
{
	// 重映射了TIM2_CH2, 所以先初始化
	// PWM_LED使用了TIM2_CH2=(PA1->PB3)
	// PWM_LED_Init();

	// TIM2_CH3(PA4) 电机初始化 
	// Motor_Init();

	// TIM2_CH1_ETR(PA0->PA15)红外线外部时钟中断, 因为和PWM_LED使用的都是TIM2, 有冲突.
	// Timer_Init();

	// TIM3_CH2=PA7 舵机初始化
	// Servo_Init();

	// // TIM2_CH1(PA0) 输出PWM波形
	// PWM_Init();
	// // 设置PWM频率: 72MHz/100/PSC
	// PWM_SetPrescaler(180);
	// // 设置PWM占空比, arg/100
	// PWM_SetCompare1(80);
	// // TIM3_CH1(PA6) 测输入频率
	// IC_Init();

	// TIM3_CH1(PA6), TIM3_CH2(PA7) TIM编码器接口 初始化
	Encoder_TIM_Init();
	// 每隔1000ms中断一次, 在中断中获取Encode的TIM3的CNT的值
	Timer_Init_Interrupt(10000);

	OLED_Init();

	OLED_ShowString(1,1,"Speed:");

	while (1)
	{
		OLED_ShowSignedNum(1,7,Speed,6);
	}
}

// TIM2的中断处理函数
void TIM2_IRQHandler(void){
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		Speed = Encoder_TIM_Get();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
