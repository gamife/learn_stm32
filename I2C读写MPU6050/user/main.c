#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
#include "LED.h"
#include <string.h>
#include "MyI2C.h"
#include "MPU6050.h"


MPU6050_DataStruct MPU6050_Data;

int main(void)
{
	OLED_Init();

	MPU6050_Init();

	uint8_t ID = MPU6050_GetID();
	OLED_ShowString(1,1, "ID:");
	OLED_ShowHexNum(1,4, ID, 2);

	while(1){
		MPU6050_GetData(&MPU6050_Data);

		OLED_ShowSignedNum(2,1,MPU6050_Data.Accx, 5);
		OLED_ShowSignedNum(3,1,MPU6050_Data.Accy, 5);
		OLED_ShowSignedNum(4,1,MPU6050_Data.Accz, 5);

		OLED_ShowSignedNum(2,8,MPU6050_Data.Gyrox, 5);
		OLED_ShowSignedNum(3,8,MPU6050_Data.Gyroy, 5);
		OLED_ShowSignedNum(4,8,MPU6050_Data.Gyroz, 5);
	}
}

