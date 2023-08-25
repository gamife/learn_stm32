#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
#include "LED.h"
#include <string.h>


Serial_Mode Serial_RxMode = RxModeString;

int main(void)
{
	OLED_Init();
	Serial_Init();
	Key_Init();
	LED_Init();

	Serial_TxPackage[0] = 0x01;
	Serial_TxPackage[1] = 0x02;
	Serial_TxPackage[2] = 0x03;
	Serial_TxPackage[3] = 0x04;

	OLED_ShowString(1,1, "TxPacket:");
	OLED_ShowString(3,1, "RxPacket:");

	while (1)
	{	
		if 	(Serial_RxMode == RxModeString){
			if (Serial_GetRxFlag() == 1){
				OLED_Clear_Line(4);
				OLED_ShowString(4,1, Serial_RxString);

				if (strcmp(Serial_RxString, "LED_ON") == 0){
					LED1_ON();
					Serial_Printf("LED_ON_OK\r\n");
					OLED_Clear_Line(2);
					OLED_ShowString(2,1, "LEND_ON_OK");

				}else if  (strcmp(Serial_RxString, "LED_OFF") == 0){
					LED1_OFF();
					Serial_Printf("LED_OFF_OK\r\n");
					OLED_Clear_Line(2);
					OLED_ShowString(2,1, "LED_OFF_OK");
				}else{
					Serial_Printf("ERR_COMMAND\r\n");
					OLED_Clear_Line(2);
					OLED_ShowString(2,1, "ERR_COMMAND");
				}
			}

			continue;
		}


		KEY_NUM	key_num = Key_GetNum();
		if (key_num != KEY_NO){
			Serial_TxPackage[0] ++;
			Serial_TxPackage[1] ++;
			Serial_TxPackage[2] ++;
			Serial_TxPackage[3] ++;
			Serial_SendPackage();
			OLED_ShowHexNum(2,1,Serial_TxPackage[0], 2);
			OLED_ShowHexNum(2,4,Serial_TxPackage[1], 2);
			OLED_ShowHexNum(2,7,Serial_TxPackage[2], 2);
			OLED_ShowHexNum(2,10,Serial_TxPackage[3], 2);
		}

		if (Serial_GetRxFlag() == 1){
			OLED_ShowHexNum(4,1,Serial_RxPackage[0], 2);
			OLED_ShowHexNum(4,4,Serial_RxPackage[1], 2);
			OLED_ShowHexNum(4,7,Serial_RxPackage[2], 2);
			OLED_ShowHexNum(4,10,Serial_RxPackage[3], 2);
		}
	}
}

