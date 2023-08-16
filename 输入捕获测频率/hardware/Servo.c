#include "stm32f10x.h"
#include "PWM.h"
#include "Servo.h"

void Servo_Init(void){
    PWM_Servo_Init();
}

/**
  * @brief  设置舵机角度
  * @param  angle: 要设置的角度. 
  *     取值范围,0-180 
  * @retval None
  */
void Servo_SetAngle(float angle){
    PWM_Servo_SetCompare(angle*2000/180 + 500);
}
