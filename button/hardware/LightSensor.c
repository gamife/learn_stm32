#include "stm32f10x.h"


// 光敏传感器初始化
void LightSensor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        // 上拉输入模式
        .GPIO_Mode = GPIO_Mode_IPU,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_13,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 获取光敏传感器的数字信号
// 返回值：1 代表光线较暗，0 代表光线较亮
// 返回值的原因:
// 光敏电阻接地, 上面串联一个电阻到VCC, 中间的输出电压, 连接到电压比较器的V+, 还有一个
// 可变电阻作为阈值电压连接到电压比较器的V-, 当光线较暗, 光敏电阻高, V+高, V+大于V-, 
// 所以电压比较器输出高电平, 反之同理.
uint8_t LightSensor_Get(void)
{
   return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
}
