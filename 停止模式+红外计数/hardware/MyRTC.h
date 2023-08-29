#ifndef __MyRTC_H
#define __MyRTC_H


extern uint16_t  MyRTC_Time[];

void MyRTC_Init(void);

void MyRTC_SetTime(void);

struct tm *MyRTC_GetTime(void);

#endif

