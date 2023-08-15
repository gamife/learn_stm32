#include "stm32f10x.h"
#include "PWM.h"


uint16_t PWM_LED_MaxPulse = 100;

// 电机PWM控制速度, TIM2_CH3(PA2)
void PWM_Motor_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使用复用推挽输出模式, 普通的推挽输出模式, 输出是由GPIO的数据输出寄存器控制的, 现在将输出控制权给到TIM.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // 使用内部时钟
    TIM_InternalClockConfig(TIM2);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 控制PWM的分辨率, 占空比的步进, 比如1%, 就是ARR=100, 如果是0.1%, 那ARR=1000
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;  // ARR 自动重装寄存器, TIM.CNT 寄存器计数到这个值, 就会自动重新计数
    // 与ARR一起控制PWM的输出频率, 72MHz/720/100 = 1Khz, 所以PWM的输出频率是 1KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 32 -1;   // PSC 预分频器, 比如内部时钟10MHz, 预分频(2-1)二分频,0表示1分频, 也就是内部时钟两个周期, 而TIM.CNT只会记一次数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    // TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    // 配置 比较输出
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 控制PWM的占空比k, 根据ARR=100, 这里CRR=50, 那么占空比就等于 50/100 ,如果ARR=1000,CRR=50, 占空比就为 50/1000
    TIM_OCInitStructure.TIM_Pulse = 0; // CCR 可用来设置占空比
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  设置TIM2_CH3电机的PWM占空比
  * @param  Compare: 
  *     取值范围,0-100
  * @retval None
  */
void PWM_Motor_SetSpeed(uint16_t Compare){
    // 设置 TIMx_CCR 寄存器, 也就是设置PWM的占空比
    TIM_SetCompare3(TIM2, Compare);
}


// LED呼吸灯, 用了重映射, 将TIM2_CH2从PA1映射到了PB3
void PWM_LED_Init(void){
    // AFIO 重映射功能:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 使用AFIO将TIM2_CH1 从PA0映射到PA15, TIM2_CH2从PA1->PB3
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
    // 由于PA15原本是JTAG调试端口, 需要禁用一下.
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使用复用推挽输出模式, 普通的推挽输出模式, 输出是由GPIO的数据输出寄存器控制的, 现在将输出控制权给到TIM.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // 使用内部时钟
    TIM_InternalClockConfig(TIM2);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 控制PWM的分辨率, 占空比的步进, 比如1%, 就是ARR=100, 如果是0.1%, 那ARR=1000
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;  // ARR 自动重装寄存器, TIM.CNT 寄存器计数到这个值, 就会自动重新计数
    // 与ARR一起控制PWM的输出频率, 7MHz/720/100 = 1Khz, 所以PWM的输出频率是 1KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 720 -1;   // PSC 预分频器, 比如内部时钟10MHz, 预分频(2-1)二分频,0表示1分频, 也就是内部时钟两个周期, 而TIM.CNT只会记一次数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    // 配置 比较输出
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 控制PWM的占空比k, 根据ARR=100, 这里CRR=50, 那么占空比就等于 50/100 ,如果ARR=1000,CRR=50, 占空比就为 50/1000
    TIM_OCInitStructure.TIM_Pulse = 0; // CCR 可用来设置占空比
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE);
}


// 驱动舵机 TIM3_ch2=PA7
// 舵机要求周期为20ms=50Hz=72Mhz/ARR/PSC
// 并且高电平宽度为0.5ms~2.5ms(对应-90度到+90度), 也就是占空比为 2.5%~12.5%, 所以10%对应了180度, 1%对应18度
// 把步进设大一些, 可能控制的会精确一些, 所以一种方式是: 步进(ARR)=20000, PSC=72, (占空比)TIM_Pulse=500-2500
void PWM_Servo_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使用复用推挽输出模式, 普通的推挽输出模式, 输出是由GPIO的数据输出寄存器控制的, 现在将输出控制权给到TIM.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // 使用内部时钟
    TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 控制PWM的分辨率, 占空比的步进, 比如1%, 就是ARR=100, 如果是0.1%, 那ARR=1000
    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;  // ARR 自动重装寄存器, TIM.CNT 寄存器计数到这个值, 就会自动重新计数
    // 与ARR一起控制PWM的输出频率, 7MHz/720/100 = 1Khz, 所以PWM的输出频率是 1KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 72 -1;   // PSC 预分频器, 比如内部时钟10MHz, 预分频(2-1)二分频,0表示1分频, 也就是内部时钟两个周期, 而TIM.CNT只会记一次数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);

    // 配置 比较输出
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 控制PWM的占空比k, 根据ARR=100, 这里CRR=50, 那么占空比就等于 50/100 ,如果ARR=1000,CRR=50, 占空比就为 50/1000
    TIM_OCInitStructure.TIM_Pulse = 0; // CCR 可用来设置占空比
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  设置TIM3舵机的PWM占空比
  * @param  Compare1: 
  *     取值范围,500-2500
  * @retval None
  */
void PWM_Servo_SetCompare(uint16_t Compare1){
    // 设置 TIMx_CCR 寄存器, 也就是设置PWM的占空比
    TIM_SetCompare2(TIM3, Compare1);
}


void PWM_LED_SetCompare2(uint16_t Compare1){
    // 设置 TIMx_CCR 寄存器, 也就是设置PWM的占空比
    TIM_SetCompare2(TIM2, Compare1);
}



