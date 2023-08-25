#include "stm32f10x.h"
#include "Delay.h"
#include "MyI2C.h"

/**
  * @brief  设置I2C的SCL输出
  * @param  BitValue: 0输出低电平, 不等于0输出高阻态(由I2C电路上拉置1)
  * @retval 无
  */
void MyI2C_W_SCL(uint8_t BitValue) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction) BitValue);
    Delay_us(10);
} 

void MyI2C_W_SDA(uint8_t BitValue) {
    GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction) BitValue);
    Delay_us(10);
} 

uint8_t MyI2C_R_SDA(void)
{
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
    Delay_us(10);
    return BitValue;
}

void MyI2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_OD,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11,
    };

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // I2C空闲状态时, SCL和SDA 都是高电平
    GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

// I2C的SCL=1时, SDA只要变化, 出现边沿, 就一定是起始条件或者终止条件, 
// 反过来, 只能通信过程中, 只能在SCL的低电平, 修改SDA
// SCL=1, SDA=1->0 起始
// SCL=1, SDA=0->1 终止 
void MyI2C_Start(void){
    // 为什么SDA先设置成1, 因为要在SCL=1时, SDA=1->0
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t byte){
    uint8_t i;
    for ( i =0; i<8; i++ ){
        MyI2C_W_SDA(byte & (0x80>>i));
        MyI2C_W_SCL(1);
        MyI2C_W_SCL(0);
    }
}

int8_t MyI2C_ReciveByte(void){
    uint8_t i, result = 0x00;
    // 主机读, 主机释放SDA, 开漏输出 
    // SCL仍由主机控制
    MyI2C_W_SDA(1);
    for (i =0; i<8; i++ ){
        MyI2C_W_SCL(1);
        // 从高位读起
        if (MyI2C_R_SDA() == 1){
            result |= (0x80 >> i);
        }
        MyI2C_W_SCL(0);
    }
    return result;
}

void MyI2C_SendAck(uint8_t AckBit){
    MyI2C_W_SDA(AckBit);
    MyI2C_W_SCL(1);
    MyI2C_W_SCL(0);
}

int8_t MyI2C_ReciveAck(void){
    uint8_t AckBit = 0;
    // 主机读, 先释放SDA
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
    AckBit = MyI2C_R_SDA();
    MyI2C_W_SCL(0);
    return AckBit;
}

