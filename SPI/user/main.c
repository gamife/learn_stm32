#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
#include "LED.h"
#include <string.h>
#include "MPU6050.h"
#include "W25Q64.h"

uint8_t ArrayWrite[] = {0xAA, 0xBB, 0xCC, 0xDD};
uint8_t ArrayRead[4];

#define W25Q64_ADDR 0x000000

int main(void)
{
	OLED_Init();

	W25Q64_Init();

	uint8_t MID;
	uint16_t DID;

 	W25Q64_ReadID(&MID, &DID);
	OLED_ShowString(1,1, "MID:   DID:");
	OLED_ShowString(2,1, "W:");
	OLED_ShowString(3,1, "R:");
	OLED_ShowHexNum(1,5, MID, 2);
	OLED_ShowHexNum(1,12, DID, 4);




	while(1){
		W25Q64_SectorErase(W25Q64_ADDR);
		W25Q64_PageProgram(W25Q64_ADDR, ArrayWrite, 4);
		W25Q64_ReadData(W25Q64_ADDR, ArrayRead, 4);

		OLED_ShowHexNum(2,3, ArrayWrite[0],2);
		OLED_ShowHexNum(2,6, ArrayWrite[1],2);
		OLED_ShowHexNum(2,9, ArrayWrite[2],2);
		OLED_ShowHexNum(2,12, ArrayWrite[3],2);

		OLED_ShowHexNum(3,3, ArrayRead[0],2);
		OLED_ShowHexNum(3,6, ArrayRead[1],2);
		OLED_ShowHexNum(3,9, ArrayRead[2],2);
		OLED_ShowHexNum(3,12, ArrayRead[3],2);
		ArrayWrite[0] ++;
		ArrayWrite[1] ++;
		ArrayWrite[2] ++;
		ArrayWrite[3] ++;
		Delay_ms(1000);
	}
}

