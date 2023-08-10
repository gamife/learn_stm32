#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "LightSensor.h"

KEY_NUM KeyNum;
int main(void)
{
	LED_Init();	
	Key_Init();
	Buzzer_Init();
	LightSensor_Init();
	
	while (1)
	{
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
