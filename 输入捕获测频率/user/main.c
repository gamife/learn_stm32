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

	// TIM2_CH1(PA0) 输出PWM波形
	PWM_Init();
	// 设置PWM频率: 72MHz/100/PSC
	PWM_SetPrescaler(180);
	// 设置PWM占空比, arg/100
	PWM_SetCompare1(80);

	// TIM3_CH1(PA6) 测输入频率
	IC_Init();

	// Encoder_Init();

	OLED_Init();

	OLED_ShowString(1,1,"Freq:00000Hz");
	OLED_ShowString(2,1,"Duty:");

	while (1)
	{
		OLED_ShowNum(1,6,IC_GetFreq(),5);
		OLED_ShowNum(2,6,IC_GetDuty(),3);
	}
}
