#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

typedef enum{
    RxModeHex,
    RxModeString,
} Serial_Mode;

extern uint8_t Serial_TxPackage[];
extern uint8_t Serial_RxPackage[];
extern char Serial_RxString[];

extern Serial_Mode Serial_RxMode;



void Serial_Init(void);

uint8_t Serial_GetRxFlag(void);

void Serial_SendPackage(void);

void Serial_SendByte(uint8_t Byte);

void Serial_SendArray(uint8_t *Array, uint16_t Length);

void Serial_SendString(char *String);

void Serial_SendNum(uint32_t Num, uint8_t Len);

void Serial_Printf(char *format, ...);

#endif
