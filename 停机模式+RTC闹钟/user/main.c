#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
#include "LED.h"
#include <string.h>
#include "MPU6050.h"
#include "W25Q64.h"
#include "MyRTC.h"
#include <time.h>


 char date_fmt[] = "%Y-%m-%e";
 char time_fmt[] = "%H:%M:%S";


int main(void)
{
	OLED_Init();
	
	MyRTC_Init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	OLED_ShowString(1,1, "CNT:");
	OLED_ShowString(2,1, "ALR:");
	OLED_ShowString(3,1, "ALRF:");

	// 初始化唤醒引脚(PA0), 出现上升沿可以使得退出待机模式
	PWR_WakeUpPinCmd(ENABLE);

	uint32_t Alarm = RTC_GetCounter() +8;
	RTC_SetAlarm(Alarm);
	OLED_ShowNum(2, 5, Alarm, 10);

	while(1){				
		OLED_ShowNum(1, 5, RTC_GetCounter(), 10);
		OLED_ShowNum(3, 6, RTC_GetFlagStatus(RTC_FLAG_ALR), 1);

		OLED_ShowString(4, 1, "Runnig");
		Delay_ms(100);
		OLED_ShowString(4, 1, "	     ");
		Delay_ms(100);
		
		OLED_ShowString(4, 1, "STANDBY");
		Delay_ms(2000);
		
		// 这里OLED清屏模拟待机前, 关闭相关的外设
		OLED_Clear();

		// 从待机模式恢复过来, 会从头开始执行代码, 因为待机模式下, cpu和ram都是没电的.
		PWR_EnterSTANDBYMode();
	}
}

