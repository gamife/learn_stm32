#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include <string.h>




int main(void)
{
	OLED_Init();

	CountSensor_Init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	OLED_ShowString(1,1, "CNT:");
	while(1){		
		uint32_t count = CountSensor_GetCount();
		OLED_ShowNum(1, 5, count, 10);

		OLED_ShowString(2,1, "Running");
		Delay_us(200);
		OLED_ShowString(2,1, "       ");
		Delay_us(200);

		// 睡眠模式
		// __WFI();

		// 停止模式只有外部中断可以唤醒
		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
		// 退出停止模式时，HSI RC振荡器被选为系统时钟, 需要重新设定系统时钟
		SystemInit();
	}
}

