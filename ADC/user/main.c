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
#include "AD.h"

uint32_t AD1;
float ADVoltage;

uint32_t AD2;
uint32_t AD3;


int main(void)
{

	AD_Init();

	OLED_Init();
	OLED_ShowString(1,1,"ADVal:");
	OLED_ShowString(2,1,"Volatge:0.00V");
	OLED_ShowString(3,1,"AD2:");
	OLED_ShowString(4,1,"AD3:");



	while (1)
	{
		AD1 = AD_GetValue(ADC_Channel_8);
		// 因为ADC的结果是12位的,也就是 0~3095映射到0~3.3V
		ADVoltage = (float)AD1 / 4095*3.3;

		AD2 = AD_GetValue(ADC_Channel_1);
		AD3 = AD_GetValue(ADC_Channel_2);
		
		OLED_ShowNum(1,7,AD1,4);
		// 显示整数部分
		OLED_ShowNum(2,9,ADVoltage,1);
		// 显示小数部分
		OLED_ShowNum(2,11, (uint16_t)(ADVoltage*100) %100,2);


		OLED_ShowNum(3,5,AD2,4);
		OLED_ShowNum(4,5,AD3,4);

		Delay_ms(100);
	}
}

