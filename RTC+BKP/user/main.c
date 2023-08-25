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

uint16_t ArrayWrite[] = {0x1234, 0x5678};
uint16_t ArrayRead[2];

 char date_fmt[] = "%Y-%m-%e";
 char time_fmt[] = "%H:%M:%S";


int main(void)
{
	OLED_Init();
	Key_Init();
	
	MyRTC_Init();


	OLED_ShowString(1,1, "W:");
	OLED_ShowString(2,1, "R:");

	OLED_ShowString(3,1,"Date:");
	OLED_ShowString(4,1,"Time:");
	
	char datetime_str[20];

	while(1){		
		struct tm *datetime;
		datetime = MyRTC_GetTime();

		strftime(datetime_str, 20, date_fmt, datetime );
		OLED_ShowString(3,6, datetime_str);

		strftime(datetime_str, 20, time_fmt, datetime );
		OLED_ShowString(4,6, datetime_str);

		KEY_NUM key = Key_GetNum();
		if (key == KEY3){
			ArrayWrite[0] ++;
			ArrayWrite[1] ++;
			BKP_WriteBackupRegister(BKP_DR1, ArrayWrite[0]);
			BKP_WriteBackupRegister(BKP_DR2, ArrayWrite[1]);

			OLED_ShowHexNum(1,3, ArrayWrite[0],4);
			OLED_ShowHexNum(1,8, ArrayWrite[1],4);
		}

		ArrayRead[0]= BKP_ReadBackupRegister(BKP_DR1);
		ArrayRead[1]= BKP_ReadBackupRegister(BKP_DR2);

		OLED_ShowHexNum(2,3, ArrayRead[0],4);
		OLED_ShowHexNum(2,8, ArrayRead[1],4);

	}
}

