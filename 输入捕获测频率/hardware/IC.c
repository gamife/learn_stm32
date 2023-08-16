#include "stm32f10x.h"
#include "IC.h"

#define CURRENT_TIM TIM3
#define CURRENT_RCC_TIM RCC_APB1Periph_TIM3
#define CURRENT_RCC_GPIO RCC_APB2Periph_GPIOA
#define CURRENT_GPIO GPIOA
#define CURRENT_PIN GPIO_Pin_6
// 除了这些, 改通道还需要修改代码的一些部分

// TIM3_CH1(PA6) 输入捕获初始化
void IC_Init(void)
{
    RCC_APB2PeriphClockCmd(CURRENT_RCC_GPIO, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使用复用推挽输出模式, 普通的推挽输出模式, 输出是由GPIO的数据输出寄存器控制的, 现在将输出控制权给到TIM.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = CURRENT_PIN;
    GPIO_Init(CURRENT_GPIO, &GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(CURRENT_RCC_TIM, ENABLE);
    // 使用内部时钟
    TIM_InternalClockConfig(CURRENT_TIM);

    // * 配置时基单元
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 控制PWM的分辨率, 占空比的步进, 比如1%, 就是ARR=100, 如果是0.1%, 那ARR=1000
    TIM_TimeBaseStructure.TIM_Period = 65536 - 1;  // ARR 自动重装寄存器, TIM.CNT 寄存器计数到这个值, 就会自动重新计数
    // 与ARR一起控制PWM的输出频率, 7MHz/720/100 = 1Khz, 所以PWM的输出频率是 1KHz
    TIM_TimeBaseStructure.TIM_Prescaler = 72 -1;   // PSC 预分频器, 比如内部时钟10MHz, 预分频(2-1)二分频,0表示1分频, 也就是内部时钟两个周期, 而TIM.CNT只会记一次数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(CURRENT_TIM, &TIM_TimeBaseStructure);
    // TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    // * 配置 捕获输入
    // 可以在手册的 <14.3.4 捕获/比较通道> 看图, 就知道下列参数的意义了.

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    // 滤波, 一般输入频率是很小于fDTS的, 所以在fDTS取几个点采样, 其实都还是在输入波形的一个周期里, 所以滤波不会改变输入的频率
    // 如果采样的几个点都是相同电平, 才会被送到边沿检测器
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    // 选择边沿检测器是 上升沿有效还是其他
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    // 设置输入捕获映射关系:
    //    直接映射(TIM_ICSelection_DirectTI): TIM3_CH1 -> CCR1(捕获/比较寄存器)
    //    交叉(TIM_ICSelection_IndirectTI): TIM3_CH1 -> CCR2
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    // 设置输入捕获分频器, 跟时基单元的预分频不是一个东西, 这个分频只有 1,2,4,8 四种
    // 设置的是每N个事件触发一次捕捉, 捕捉就是触发将CNT的值写到CCR寄存器
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;

    // ** 下面一段相同的IC初始化配置, 实际等于这一行 TIM_PWMIConfig()
    // TIM_PWMIConfig(TIM3,  &TIM_ICInitStructure)

    TIM_ICInit(CURRENT_TIM, &TIM_ICInitStructure);
    // 这里是捕获到 CCR2 的意思
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    // 这里是相反的 下降沿触发捕获
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    // 交叉: 也就是 CCR2 由TIM3_CH1 提供输入信号
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInit(CURRENT_TIM, &TIM_ICInitStructure);

    // * 配置从模式
    // 从模式触发源选择
    TIM_SelectInputTrigger(CURRENT_TIM, TIM_TS_TI1FP1);
    // 选择触发后的动作, 这里是触发捕获后CNT清零
    TIM_SelectSlaveMode(CURRENT_TIM, TIM_SlaveMode_Reset);

    // * 启动定时器
    TIM_Cmd(CURRENT_TIM, ENABLE);
}


/**
  * @brief  获取输入信号的频率
  * @param  None
  * @retval 输入信号的频率
  */
uint32_t IC_GetFreq(void){
    // 72MHz/PSC/(输入信号一个周期有多少个内部周期)
    // 加一是因为,不知道为啥 测试自己产生的1000Hz的PWM信号, 结果是1001Hz
    return 72000000/72/(TIM_GetCapture1(CURRENT_TIM)+1);
}

/**
  * @brief  获取输入信号的占空比
  * @param  None
  * @retval 输入信号的占空比
  */
uint32_t IC_GetDuty(void){
    // 占空比 = (上升沿->下降沿的计数) 除以 (上升沿->上升沿的计数)
    // 乘以100是为了显示整数.
    // 两个加一都是因为,不知道为啥 测试自己产生的1000Hz的PWM信号, 结果是1001Hz
    return 100 * (TIM_GetCapture2(CURRENT_TIM) + 1)/(TIM_GetCapture1(CURRENT_TIM)+1);
}
