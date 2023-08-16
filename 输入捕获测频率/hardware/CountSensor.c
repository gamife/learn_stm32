#include "stm32f10x.h"

uint32_t AllCount = 0;

// 红外传感器PB14 引脚遮挡中断计数
void CountSensor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        // 上拉输入模式
        .GPIO_Mode = GPIO_Mode_IPU,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_14,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // EXTI中断:
    // EXTI有类似20个引脚, 也就是EXTI_Line, GPIO的pin与EXTI_Line是多对一的关系, 比如 (PA15,PB15...PG15)->Line15, 可以选择某一个GPIOx的pin15作为中断引脚,
    // 假如初始化了EXTI_Line15, 而且是GPIOA的pin15, 连上了EXTI_Line15, 然后配置的下降沿触发, 那么当PA15有一个下降沿, 那么EXTI就会中断发给NVIC, NVIC通过配置的优先级执行中断.
    // 所以每个GPIOx的pin作为中断引脚时, pin的序号不能相同, 比如用了GPIOA的pin1作为中断引脚, 就不能再绑定GPIOB的pin1作为中断引脚了.
 
    // 1.将PB14连上Line14
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,   GPIO_PinSource14);
    // 2. 使能Line14的中断, 配置下降沿触发
    EXTI_InitTypeDef EXTI_InitStructure = {
        .EXTI_Line = EXTI_Line14,
        .EXTI_Mode = EXTI_Mode_Interrupt,
        .EXTI_Trigger = EXTI_Trigger_Falling,
        .EXTI_LineCmd = ENABLE,
    };
    EXTI_Init(&EXTI_InitStructure);

    // 优先级分组配置. 总共有4bit来配置, 这里选择了NVIC_PriorityGroup_2, 也就是:
    // 2个bit设置抢占优先类型, 也就是抢占优先类型现在有四个等级, 可以把上百种中断源划分到这四个等级, 高等级可以在低等级中断执行的时候中断去高等级中断的代码, 嵌套中断.
    // 2个bit设置subpriority类型, 同样2^2四个等级, 可以把上百种中断源划分到这四个等级, 不是嵌套中断, 会等待当前中断执行完, 然后再先执行高等级中断.
    // 我理解, 如果有一个新产生的中断, NVIC先判断是否嵌套已有的中断(根据抢占式优先级), 如果抢占式优先级相同, 则再根据subpriority优先级排队.
    // 整个代码只要配置一次, 可以放到main.c里去
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {
        // GPIO引脚的中断属于EXTI中断类型, 其他还有时钟中断类型, 以太网中断类型等等
        .NVIC_IRQChannel = EXTI15_10_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 2,
        .NVIC_IRQChannelSubPriority = 1,
        .NVIC_IRQChannelCmd = ENABLE,
    };
    NVIC_Init(&NVIC_InitStructure);
}

uint32_t CountSensor_GetCount(void){
    return AllCount;
}
// EXTI15_10中断类型的中断处理函数
// start目录的startup_stm32f10x_md.s的__Vectors有一个中断函数表, 当有中断发生, NVIC就会跳到对应的处理函数
void EXTI15_10_IRQHandler(void){
    // 因为EXTI15_10中断类型, 可能是EXTI的Line10-15的中断, 要判断一下是不是pin14触发的中断
    if (EXTI_GetITStatus(EXTI_Line14) == SET){
        // 撤离遮挡, 是下降沿, 从1到0, 触发中断, 为了消除抖动, 再查看下引脚是否是0
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0){
            AllCount ++;
        }
        // 中断处理完, 要清除对应的中断标志位, 否则会反复执行中断函数
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}
