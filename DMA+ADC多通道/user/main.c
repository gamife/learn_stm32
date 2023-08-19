#include "stm32f10x.h"                  // Device header
#include "Delay.h"
// #include "LED.h"
// #include "Key.h"
// #include "Buzzer.h"
// #include "LightSensor.h"
#include "OLED.h"
// #include "CountSensor.h"
// #include "Encoder.h"
// #include "Timer.h"
// #include "PWM.h"
// #include "Servo.h"
// #include "Motor.h"
// #include "IC.h"
// #include "EncoderTIM.h"
#include "AD.h"
// #include "MyDMA.h"


int main(void)
{

	OLED_Init();

	AD_Init();

	while (1)
	{
		OLED_ShowNum(1,5,AD_Values[0],4);
		OLED_ShowNum(2,5,AD_Values[1],4);
		OLED_ShowNum(3,5,AD_Values[2],4);
		Delay_ms(1000);
	}
}

