#include "stm32f10x.h"
#include "MyDMA.h"

uint16_t MyDMA_LEN;

void MyDMA_Data_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Len){
    MyDMA_LEN = Len;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;

    // 指定传输方向, 这里DMA_DIR_PeripheralSRC指定Peripheral(外设)作为源地址, 实际这里的外设地址不一定非得是外设, 指定任意可读的地址都行.
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

    // 指定源地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA;
    // 源数据宽度, 每次传输此bit数量的数据
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    // 是否自增, 就是每传一次数据, 下一次传输时, 源地址要不要增加一个数据宽度
    // 如果源地址是程序的一个数组, 每次传输数组的一个元素, 那就需要自增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    // 目的地址
    DMA_InitStructure.DMA_MemoryBaseAddr = AddrB;
    // 目的数据宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // 是否自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    // 传输次数
    DMA_InitStructure.DMA_BufferSize = Len;
    // 是否软件触发DMA, 如果是的, 那么DMA的计数就会尽可能快的减, 也就是尽可能快的复制完, 
    // 比如程序里数组拷贝到数组, 就可以很快的进行, 而不是像定时器或其他外设触发一样, 需要某种条件, 才会触发一次传输.
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    // 是否自动重装, 也就是传输次数递减到0的时候, 又重新开始传输. 不能和软件触发同时启动, 因为这样的话, 无休止的传输了.
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    // 转运的优先级, 因为一个DMA有多个通道.
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;

    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void MyDMA_Transfer(void){
    // 注意: DMA重新计数需要先关闭DMA
    DMA_Cmd(DMA1_Channel1, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel1, MyDMA_LEN);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 等待转运完成标志位
    while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
    // 清除标志位
    DMA_ClearFlag(DMA1_FLAG_TC1);
}
