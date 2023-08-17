#include "stm32f10x.h"
#include "EncoderTIM.h"

#define CURRENT_TIM TIM3
#define CURRENT_RCC_TIM RCC_APB1Periph_TIM3
#define CURRENT_RCC_GPIO RCC_APB2Periph_GPIOA
#define CURRENT_GPIO GPIOA
#define CURRENT_PIN GPIO_Pin_6|GPIO_Pin_7

// TIM3_CH1(PA6), TIM3_CH2(PA7) TIM编码器接口 初始化
void Encoder_TIM_Init(void)
{
    RCC_APB2PeriphClockCmd(CURRENT_RCC_GPIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使用复用推挽输出模式, 普通的推挽输出模式, 输出是由GPIO的数据输出寄存器控制的, 现在将输出控制权给到TIM.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = CURRENT_PIN;
    GPIO_Init(CURRENT_GPIO, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(CURRENT_RCC_TIM, ENABLE);
    // 编码器本身就类似一个外部时钟
    // TIM_InternalClockConfig(CURRENT_TIM);

    // * 配置时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 65536 - 1;
    // 旋转编码器的输入 直接控制CNT, 选择不使用预分频
    TIM_TimeBaseStructure.TIM_Prescaler = 1 -1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 没用, 因为编码器接口模式, CNT是托管给 编码器接口的
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(CURRENT_TIM, &TIM_TimeBaseStructure);

    // 编码器接口模式 只会用到 滤波和边沿触发器
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    // 滤波, 一般输入频率是很小于fDTS的, 所以在fDTS取几个点采样, 其实都还是在输入波形的一个周期里, 所以滤波不会改变输入的频率
    // 如果采样的几个点都是相同电平, 才会被送到边沿检测器
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    // 对于编码器接口, 上升沿和下降沿都有效, 所以这个参数实际是选择是否将输入反向, TIM_ICPolarity_Rising=不反向
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInit(CURRENT_TIM, &TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInit(CURRENT_TIM, &TIM_ICInitStructure);

    // 配置编码器接口模式
    // 第二个参数: 1.只关注TI1的边沿 2.只关注TI2的边沿 3.两者都关注 (TI1 这里类CH1也就是一个输入引脚)
    // 第三四参数: 实际作用和上面初始化IC的TIM_ICPolarity是重复的, 都是是否反向的意思. 如果输入的两个引脚接反了, 可以把其中一个改为 TIM_ICPolarity_Falling
    TIM_EncoderInterfaceConfig(CURRENT_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // * 启动定时器
    TIM_Cmd(CURRENT_TIM, ENABLE);
}


/**
  * @brief  获取CNT, 同时清零CNT
  * @param  None
  * @retval Counter Register value.
  */
int16_t Encoder_TIM_Get(void){
  int16_t temp;
  temp = TIM_GetCounter(CURRENT_TIM);
  TIM_SetCounter(CURRENT_TIM, 0);
  return temp;
}
