#include "stm32f10x.h"
#include "MySPI_Hard.h"

/**
  * @brief  写入SS从机选择. 
  *         选择与哪个从机通信, 每个从机各自有SS线与主机相连. 这里只与W25Q64闪存通信
  * @param  BitValue: 非0高电平=空闲 0低电平=通信
  * @retval None
  */
void MySPI_W_SS(uint8_t BitValue) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);
}


void MySPI_Hard_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    // SPI主机的CLK PA5(时钟线),MOSI PA7(主机输出) 都是硬件推挽输出
    GPIO_InitStructure  .GPIO_Mode = GPIO_Mode_AF_PP,
    GPIO_InitStructure  .GPIO_Speed = GPIO_Speed_50MHz,
    GPIO_InitStructure  .GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_7,
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // SS PA4(从机选择) 还是由软件控制GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // MISO(主机输入) 为上拉或者浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitTypeDef SPI_InitStructure;
    // 双线全双工
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    // stm32作为SPI的主机
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    // 8位数据帧
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    // 高位先行
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    // 波特率分频器, 控制SPI的时钟频率, 注意SPI1和SPI2接在不同的总线上, APB2=72MHz APB1=32MHz
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    // SPI模式0, SCK空闲状态为低电平
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    // SPI模式0, 在SS下降沿就写数据, 在SCK的第一个边沿就 读数据了,
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    // 选择软件模拟SS
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    // CRC数据校验, 这里给个默认值
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // SS空闲为高电平
    MySPI_W_SS(1);

    SPI_Cmd(SPI1, ENABLE);
}

void MySPI_Start(void)
{
    MySPI_W_SS(0);
}

void MySPI_Stop(void)
{
    MySPI_W_SS(1);
}

uint8_t MySPI_SwapByte(uint8_t ByteSend){
    // 这里使用的是stm32的SPI硬件的非连续传输模式, 

    // 等待数据发送寄存器为空
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, ByteSend);

    // 等待接收到数据
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);
    return SPI_I2S_ReceiveData(SPI1);
}
