#include "stm32f10x.h"
#include "AD.h"

// ADC_IN8(PB0),ADC1_IN1(PA1),ADC1_IN2(PA2)ADC初始化
void AD_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        // 是GPIO无效, 用于模拟输入
        .GPIO_Mode = GPIO_Mode_AIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_0,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure .GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2,

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // ADC的时钟最大14Mhz, 所以只能选6/8分频 72MHz/6 = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // 第三个参数: 就是将ADC_CH8(PB0)引脚, 放到规则组的第一个位置, 这个规则组最多16个位置, 可以不同位置放相同ADC_CH, 
    //  按位置顺序依次采样, 采样就是锁住某个模拟电压输入, 然后通过二分法在0~3.3V范围里, 比较出输入电压的估计值, 然后依次将结果放入 规则组寄存器,
    //  但是规则组寄存器只有一个, 所以如果规则组有多个ADC通道待测量, 后续的结果会覆盖前面的结果(需要DMA去保存结果),
    //  还有四个注入组, 注入组各有一个寄存器保存结果.
    // 第四个参数: 采样的时钟周期, 越长越稳定些.
    // ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

    ADC_InitTypeDef ADC_InitStructure;
    // 选择ADC单通道, 只有一个ADC引脚
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    // 数据对其模式, 指定右对齐(高位补0), 因为ADC的结果精度只有12位, 保存到16位的寄存器.
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    // 触发模式, 可以是硬件触发, 比如定时器触发, 也可以是软件触发, 这里选择软件触发ADC_ExternalTrigConv_None
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    // 扫描模式: 规则组里可以填最多16个ADC通道, 如果开启扫描模式, 那么会依次将最多16个模拟输入进行转换(数量可以指定), 非扫描模式, 就只会转换第一个输入.
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    // 扫描模式时才有效, 指定每一次要从1转换到第N个通道(最多16)
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    // 是否连续转换, 也就是触发一次, 但是规则组测完一遍又自动来一遍. 
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    // 复位校准ADC
    ADC_ResetCalibration(ADC1);
    // 等待复位校准完成 ADC_CR2.RSTCAL=1 表示正在初始化校准寄存器
    while (ADC_GetResetCalibrationStatus(ADC1) != RESET);
    // 开始校准ADC
    ADC_StartCalibration(ADC1);
    // 等待校准完成, ADC_CR2.CAL=1 表示开始校准, 0为校准完成
    while (ADC_GetCalibrationStatus(ADC1)  != RESET);
}


/**
  * @brief  获取ADC规则组结果
  * @param  ADC_Channel: 要转换的ADC通道(记得在init函数初始化GPIO)
  * @retval ADC转换结果
  */
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
    // ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);

    // 软件触发ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // 等待转换完成
    // ADC_SR.EOC=1表示转换完成, 由软件置0或者读取规则组结果(ADC_DR寄存器)时硬件清除
    // 转换总时间 = 采样时间 + ADC二分法逼近时间(12位结果)
    // 所以总时间 = 55.5 + 12.5(固定的,为什么+0.5不知道) = 68个ADC周期 / (72M / 6分频)
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
    // 读取规则组结果(ADC_DR寄存器)
    return ADC_GetConversionValue(ADC1);
}
