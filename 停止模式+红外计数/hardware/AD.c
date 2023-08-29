#include "stm32f10x.h"
#include "AD.h"


uint16_t AD_Values[3];
uint8_t ADC_Channel_Num = 3;

// ADC_IN8(PB0),ADC1_IN1(PA1),ADC1_IN2(PA2)ADC初始化
void AD_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // ADC的时钟最大14Mhz, 所以只能选6/8分频 72MHz/6 = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    GPIO_InitTypeDef GPIO_InitStructure = {
        // 是GPIO无效, 用于模拟输入
        .GPIO_Mode = GPIO_Mode_AIN,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_0,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化规则组要进行AD转换的ADC通道
    // 第三个参数: 就是将ADC_CH8(PB0)引脚, 放到规则组的第一个位置, 这个规则组最多16个位置, 可以不同位置放相同ADC_CH, 
    //  按位置顺序依次采样, 采样就是锁住某个模拟电压输入, 然后通过二分法在0~3.3V范围里, 比较出输入电压的估计值, 然后依次将结果放入 规则组寄存器,
    //  但是规则组寄存器只有一个, 所以如果规则组有多个ADC通道待测量, 后续的结果会覆盖前面的结果(需要DMA去保存结果),
    //  还有四个注入组, 注入组各有一个寄存器保存结果.
    // 第四个参数: 采样的时钟周期, 越长越稳定些.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_55Cycles5);

    ADC_InitTypeDef ADC_InitStructure;
    // 选择ADC单通道, 只有一个ADC引脚
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    // 数据对其模式, 指定右对齐(高位补0), 因为ADC的结果精度只有12位, 保存到16位的寄存器.
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    // 触发模式, 可以是硬件触发, 比如定时器触发, 也可以是软件触发, 这里选择软件触发ADC_ExternalTrigConv_None
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    // 扫描模式: 规则组里可以填最多16个ADC通道, 如果开启扫描模式, 那么会依次将最多16个模拟输入进行转换(数量可以指定), 非扫描模式, 就只会转换第一个输入.
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    // 扫描模式时才有效, 指定每一次要从1转换到第N个通道(最多16)
    ADC_InitStructure.ADC_NbrOfChannel = ADC_Channel_Num;
    // 是否连续转换, 也就是触发一次, 但是规则组测完一遍又自动来一遍. 
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 2. 初始化DMA
    DMA_InitTypeDef DMA_InitStructure;
    // 指定传输方向, 这里DMA_DIR_PeripheralSRC指定Peripheral(外设)作为源地址, 实际这里的外设地址不一定非得是外设, 指定任意可读的地址都行.
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    // 2.1
    // 指定源地址
 	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    // 源数据宽度, 每次传输此bit数量的数据, ADC的结果是12位放在16位的ADC_DR寄存器
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    // 是否自增, 就是每传一次数据, 下一次传输时, 源地址要不要增加一个数据宽度
    // 如果源地址是程序的一个数组, 每次传输数组的一个元素, 那就需要自增
    // 这里ADC的规则组的所有结果总是覆盖的放在ADC_DR寄存器, 所以不用自增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // 2.2
    // 目的地址
    // DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Values;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Values;
    // 目的数据宽度, 注意不是外设的DMA_PeripheralDataSize_HalfWord
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    // 是否自增, 这里每次把ADC_DR的数据放到数组里, 数组需要自增, 否则就覆盖了.
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 2.3
    // 传输次数
    DMA_InitStructure.DMA_BufferSize = ADC_Channel_Num;
    // 是否软件触发DMA, 如果是的, 那么DMA的计数就会尽可能快的减, 也就是尽可能快的复制完, 
    // 比如程序里数组拷贝到数组, 就可以很快的进行, 而不是像定时器或其他外设触发一样, 需要某种条件, 才会触发一次传输.
    // 这里由ADC硬件来触发, 所以disable
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    // 是否自动重装, 也就是传输次数递减到0的时候, 又重新开始传输. 不能和软件触发同时启动, 因为这样的话, 无休止的传输了.
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    // 转运的优先级, 因为一个DMA有多个通道.
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;

    // 这里只能是 DMA1_CH1, 因为根据手册 ADC1只能接在通道1
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 3. 将ADC信号给到DMA
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    // 复位校准ADC
    ADC_ResetCalibration(ADC1);
    // 等待复位校准完成 ADC_CR2.RSTCAL=1 表示正在初始化校准寄存器
    while (ADC_GetResetCalibrationStatus(ADC1) != RESET);
    // 开始校准ADC
    ADC_StartCalibration(ADC1);
    // 等待校准完成, ADC_CR2.CAL=1 表示开始校准, 0为校准完成
    while (ADC_GetCalibrationStatus(ADC1)  != RESET);

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
