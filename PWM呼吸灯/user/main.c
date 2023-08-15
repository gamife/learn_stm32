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

int32_t Timer_Interrupt_Num = 0;
void TIM2_IRQHandler(void){
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		Timer_Interrupt_Num ++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}


// PWM的LED呼吸灯, 因阻塞严重, 直接改写在main函数里了.
void PWM_LED(void){
	for (int i = 0; i <= PWM_LED_MaxPulse; i++){
		PWM_LED_SetCompare2(i);
		Delay_ms(1000/PWM_LED_MaxPulse);
	}
	for (int i = 0; i <= PWM_LED_MaxPulse; i++){
		PWM_LED_SetCompare2(PWM_LED_MaxPulse - i);
		Delay_ms(1000/PWM_LED_MaxPulse);
	}
}


int main(void)
{
	// 重映射了TIM2_CH2, 所以先初始化
	// PWM_LED使用了TIM2_CH2=(PA1->PB3)
	// PWM_LED_Init();

	// TIM2_CH3(PA4) 电机初始化 
	Motor_Init();

	// TIM2_CH1_ETR(PA0->PA15)红外线外部时钟中断, 因为和PWM_LED使用的都是TIM2, 有冲突.
	// Timer_Init();

	// TIM3_CH2=PA7 舵机初始化
	Servo_Init();

	Encoder_Init();

	OLED_Init();

	// OLED_ShowString( 1,1,"hello stm32!");
	// Delay_ms(2000);
	// OLED_Clear();

	OLED_ShowString(1,1,"Angle:");
	OLED_ShowString(2,1,"Encode:");
	OLED_ShowString(3,1,"TimerI:");
	OLED_ShowString(4,1,"Speed:");

	KEY_NUM KeyNum;
	int32_t EncoderNum = 0;
	int32_t ServoAngle = 0;
	int32_t MotorSpeed = 0;
	int32_t Current_Encoder = 0;
	int32_t LED_i = 0;
	int8_t LED_Increment_Flag = 1;
	while (1)
	{
		Current_Encoder = Encoder_Get();
		EncoderNum += Current_Encoder;

		MotorSpeed += 10 * Current_Encoder;
		if (MotorSpeed > 100){
			MotorSpeed = 0;
		}else if (MotorSpeed < -100){
			MotorSpeed = 0;
		}

		ServoAngle += 15.0 * Current_Encoder;
		if (ServoAngle > 180){
			ServoAngle = 180;
		}else if (ServoAngle < 0){
			ServoAngle = 0;
		}

		OLED_ShowNum(1,7,ServoAngle,3);
		OLED_ShowSignedNum(2,8,EncoderNum,6);
		OLED_ShowNum(3,8,Timer_Interrupt_Num,6);
		OLED_ShowSignedNum(4,8,MotorSpeed,6);
		
		// 控制舵机角度
		Servo_SetAngle(ServoAngle);
		// 控制电机速度及方向
		Motor_SetSpeed(MotorSpeed);

		// PWM LED
		// if (LED_i <= PWM_LED_MaxPulse){
		// 	PWM_LED_SetCompare2(LED_i);
		// 	// Delay_ms(100/PWM_LED_MaxPulse);
		// 	if (LED_Increment_Flag){
		// 		LED_i ++;
		// 		if (LED_i == PWM_LED_MaxPulse){
		// 			LED_Increment_Flag = 0;
		// 		}
		// 	}else{
		// 		LED_i --;
		// 		if (LED_i == 0){
		// 			LED_Increment_Flag = 1;
		// 		}
		// 	}
		// }

	}
}
