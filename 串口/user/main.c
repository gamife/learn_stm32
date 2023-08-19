#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

uint8_t RxData;

int main(void)
{
	OLED_Init();
	Serial_Init();
	Serial_SendByte(0x41);

	uint8_t MyArray[] = {0x42,0x43,0x44,0x45};
	Serial_SendArray(MyArray,4);

	Serial_SendString("\r\nHello World!\r\n");
	Serial_SendNum(12345, 5);

	printf("\r\nNum=%d\r\n", 666);

	Serial_Printf("Num=%d\r\n", 888);
	Serial_Printf("你好, 世界.\r\n");
	
	while (1)
	{
		if(Serial_GetRxFlag()  == 1){
			RxData =  Serial_GetRxData();
			Serial_SendByte(RxData);
			OLED_ShowNum(1,1, RxData, 2);
		}
	}
}

