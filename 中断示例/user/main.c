#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "LightSensor.h"
#include "OLED.h"
#include "CountSensor.h"
#include "Encoder.h"


int main(void)
{
	CountSensor_Init();
	Encoder_Init();

	OLED_Init();
	LED_Init();	
	Key_Init();
	Buzzer_Init();
	LightSensor_Init();

	OLED_ShowString( 1,1,"hello stm32!");
	Delay_ms(2000);
	OLED_Clear();

	OLED_ShowString(1,1,"Count:");
	OLED_ShowString(2,1,"Encode:");

	KEY_NUM KeyNum;
	int32_t EncoderNum = 0;

	while (1)
	{
		EncoderNum += Encoder_Get();
		OLED_ShowNum(1,8,CountSensor_GetCount(),6);
		OLED_ShowSignedNum(2,8,EncoderNum,6);

		if (LightSensor_Get() == 1){
			Buzzer_OFF();
		}else{
			// 亮光, 蜂鸣器响
			Buzzer_ON();
		}

		KeyNum = Key_GetNum();
		
		if (KeyNum != KEY_NO)
		{
			Buzzer_ON();
			Delay_ms(100);
			Buzzer_OFF();
			Delay_ms(100);
			Buzzer_Turn();
			Delay_ms(100);
			Buzzer_Turn();
		}

		if (KeyNum == KEY1)
		{
			LED_Turn(GPIO_Pin_1);
		}
		
		if (KeyNum == KEY2)
		{
			LED_Turn(GPIO_Pin_2);
		}
	}
}
