#include "stm32f10x.h"
#include "Timer.h"

// 红外线外部时钟中断 TIM2_CH1_ETR(PA0->PA15)
void Timer_Init(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    //使能TIM2时钟
    
    // 使用GPIO的PB0作为外部时钟输入引脚, 所以要初始化GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
    GPIO_InitTypeDef GPIO_InitStructure;
    // 手册上建议使用浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // * 使用内部时钟
    // TIM_InternalClockConfig(TIM2);
    // * 使用外部时钟
    // 第四个参数(还需要再看下): 数字滤波, 防止抖动产生多个下降沿(时钟计数多次)
    //  0x00的时候, 我用红外传感器, 遮挡一次, 会产生好几个不确定的时钟周期, 改成0x0F, 每遮挡一次, 时钟只记一次数
    //  这个参数和时基单元的TIM_ClockDivision还有些关系, 看TIMx_SMCR.ETF和TIMx_CR1.CKD, 注意, 外部时钟的相位和芯片主时钟是有差异的.
    // 第二个参数,TIM_ExtTRGPSC_OFF: 时钟模式2, 在外部时钟作为时基单元的输入前, 会有一次分频, 由TIMx_SMCR寄存器来配置. 因为外部触发信号ETRP的频率必须最多是CK_INT(内部时钟)频率的1/4。

    TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1-1; //预分频
    TIM_TimeBaseInitStructure.TIM_Period = 10-1;  //自动重装 
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    // 配置时基单元
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure); 
    // 在TIMx_EGR寄存器中(通过软件方式或者使用从模式控制器)设置UG位也同样可以产生一个更新事件, 
    // 这个更新事件使得TIM_SR寄存器的UIF置位, 表示有中断要处理, 然后NVIC中断使能后, 就会立马执行中断函数.
    // 而上面的TIM_TimeBaseInit函数里就设置了UG位, 导致开启中断, 就立即有一个TIM2的中断
    // 故NVIC开启TIM2中断前, 清理一下UIF标志
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  //使能定时器2中断

    // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 设置中断优先级分组2
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // 定时器2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);  // 使能定时器2
}

uint16_t Timer_GetCounter(void){
    return TIM_GetCounter(TIM2);
}
// void TIM2_IRQHandler(void){
//     if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
//         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//     }
// }
