#include "stm32f10x.h"
#include "MySPI.h"

// 这几个直接操作SPI引脚的函数, 不用延时是因为, W25Qxx系列的闪存时钟频率为80Mhz, 比stm32f103c8芯片的主频还快, 所以没必要延时.

/**
  * @brief  写入SS从机选择. 
  *         选择与哪个从机通信, 每个从机各自有SS线与主机相连. 这里只与W25Q64闪存通信
  * @param  BitValue: 非0高电平=空闲 0低电平=通信
  * @retval None
  */
void MySPI_W_SS(uint8_t BitValue) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);
}

/**
  * @brief  写入SCK时钟.
  *         对于SPI不同的模式, 空闲可能是低或者高电平. 对于SPI模式0, 空闲是低电平
  * @param  BitValue: 非0=高电平 0=低电平
  * @retval None
  */
void MySPI_W_SCK(uint8_t BitValue) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)BitValue);
}

/**
  * @brief  写入一个bit输出数据.
  * @param  BitValue: 非0=高电平 0=低电平
  * @retval None
  */
void MySPI_W_MOSI(uint8_t BitValue) {
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)BitValue);
}

/**
  * @brief  读出一个bit输入数据.
  * @param  None 
  * @retval 读出的bit
  */
uint8_t MySPI_R_MISO(void) {
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}

void MySPI_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    // SPI主机的CLK(时钟线), SS(从机选择), MOSI(主机输出) 都是推挽输出, 有驱动能力, 跟I2C不同
    GPIO_InitStructure  .GPIO_Mode = GPIO_Mode_Out_PP,
    GPIO_InitStructure  .GPIO_Speed = GPIO_Speed_50MHz,
    GPIO_InitStructure  .GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7,
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // MISO(主机输入) 为上拉或者浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // SS空闲为高电平
    MySPI_W_SS(1);
    // SCK 对于模式0, 空闲为低电平
    MySPI_W_SCK(0);
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
    uint8_t i, ByteRecive = 0;
    // SPI高位先行, 同I2C
    for (i = 0; i < 8; i++) {
        // SPI模式0, 在SCK下降沿(低电平)写数据, 上升沿(高电平)读数据
        // 另外, 模式0会在SS刚出现下降沿时, 就写数据了.
        MySPI_W_MOSI(ByteSend & (0x80 >> i));
        MySPI_W_SCK(1);
        if  (MySPI_R_MISO() == 1) { ByteRecive |= (0x80 >> i);}
        MySPI_W_SCK(0);
    }

    return ByteRecive;
}
