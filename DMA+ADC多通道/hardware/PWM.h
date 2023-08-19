#ifndef __PWM_H
#define __PWM_H

extern uint16_t  PWM_LED_MaxPulse;

void PWM_Motor_Init(void);
void PWM_Motor_SetSpeed(uint16_t Compare);

void PWM_LED_Init(void);
void PWM_LED_SetCompare2(uint16_t Compare1);

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare1);
void PWM_SetPrescaler(uint16_t Prescaler);

void PWM_Servo_Init(void);
void PWM_Servo_SetCompare(uint16_t Compare1);

#endif

