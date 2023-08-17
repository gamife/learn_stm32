#ifndef __TIMER_H
#define __TIMER_H

void Timer_Init(void);

uint16_t Timer_GetCounter(void);

void Timer_Init_Interrupt(uint16_t Period);

#endif

