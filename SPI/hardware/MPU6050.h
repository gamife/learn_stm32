#ifndef __MCU6050_H
#define __MCU6050_H

typedef struct
{
    int16_t Accx;
    int16_t Accy;
    int16_t Accz;
    int16_t Gyrox;
    int16_t Gyroy;
    int16_t Gyroz;
}MPU6050_DataStruct;

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);

uint8_t MPU6050_ReadReg(uint8_t RegAddress);

void MPU6050_Init(void);

uint8_t MPU6050_GetID(void);

void MPU6050_GetData(MPU6050_DataStruct* data);

#endif

