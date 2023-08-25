#include "stm32f10x.h"
#include "Serial.h"
#include <stdio.h>
#include <stdarg.h>


uint8_t Serial_RxFlag;
uint8_t Serial_RxPackage[4];
uint8_t Serial_TxPackage[4];
char Serial_RxString[100];


// tx(PA9), rx(PA8)
void Serial_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {
        // 串口发送默认是高电平, 低电平是起始位
        .GPIO_Mode = GPIO_Mode_AF_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_Pin = GPIO_Pin_9,
    };
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    // 数据字节长度, 有校验位就选9
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    // 停止位占几个时钟周期
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    // 校验模式.
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    // 是否使用流控, 流控就是怕接收端处理不过来, 所以发送端发送的时候会问一下
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    // 配置串口数据接收的中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelCmd = ENABLE,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority = 1,
    };
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

// 串口中断处理函数
void USART1_IRQHandler(void){
    static uint8_t RxState  = 0;
    static uint8_t pRxPackage  = 0;
    static uint8_t RxData;

    // 如果是数据接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
        RxData = USART_ReceiveData(USART1);

        // 如果是字节模式, 接收就是固定4个字节
        if (Serial_RxMode == RxModeHex){
            if (RxState == 0){
                if (RxData == 0xFF){
                    RxState = 1;
                    pRxPackage = 0;
                }
            }
            else if (RxState == 1){
                Serial_RxPackage[pRxPackage] = RxData;
                pRxPackage++;
                if (pRxPackage == 4){
                    RxState = 2;
                }
            }
            else if (RxState == 2){
                if (RxData == 0xFE){
                    RxState = 0;
                    Serial_RxFlag = 1;
                }
            }
        }else{
            if (RxState == 0){
                if (RxData == '@'){
                    RxState = 1;
                    pRxPackage = 0;
                }
            }
            else if (RxState == 1){
                if (RxData == '\r'){
                    RxState = 2;
                }else{
                    Serial_RxString[pRxPackage] = RxData;
                    pRxPackage++;
                }
            }
            else if (RxState == 2){
                if (RxData == '\n'){
                    RxState = 0;
                    Serial_RxString[pRxPackage] = '\0';
                    Serial_RxFlag = 1;
                }
            }
        }

        // 这里可以不清楚, 如果读取了数据, 那么硬件会自动清除
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/**
  * @brief  查询串口是否接收到新数据
  * @param  None
  * @retval 1=接收到新数据
  */
uint8_t Serial_GetRxFlag(void){
    if (Serial_RxFlag == 1){
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}

/**
  * @brief  发送自定义数据包
  * @param  None
  * @retval None
  */
void Serial_SendPackage(void){
    Serial_SendByte(0xFF);
    Serial_SendArray(Serial_TxPackage,4);
    Serial_SendByte(0xFE);
}

/**
  * @brief  串口发送字节
  * @param  Byte: 要发送的数据. 
  * @retval None
  */
void Serial_SendByte(uint8_t Byte){
    USART_SendData(USART1, Byte);
    // USART_SR.TXE 标志这数据从发送数据寄存器 转移到了移位寄存器, 随后由电路 移位寄存器从低位, 发送出去
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
}

/**
  * @brief  串口发送数组
  * @param  Array: 要发送的数组地址. 
  * @param  Length: 要发送的数组长度. 
  * @retval None
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length){
    uint16_t i;
    for (i = 0; i < Length; i++){
        Serial_SendByte(Array[i]);    
    }
        
}
/**
  * @brief  串口发送字符串
  * @param  String: 要发送的数组字符串. 
  * @retval None
  */
void Serial_SendString(char *String){
    uint16_t i;
    for (i = 0; String[i] != 0; i++){
        Serial_SendByte(String[i]);    
    } 

}

uint32_t Pow(uint32_t x, uint32_t y){
    uint32_t Result = 1;
    while (y --){
        Result *= x;
    }
    return Result;
}

/**
  * @brief  串口发送数字
  * @param  Num: 要发送的数字. 
  * @param  Len: 要发送的数字10进制长度. 
  * @retval None
  */
void Serial_SendNum(uint32_t Num, uint8_t Len){
    uint16_t i;
    for (i = 0; i< Len; i++){
        Serial_SendByte(Num / Pow(10, Len - i - 1) % 10 + '0');    
    }

}

int fputc (int ch ,FILE *f){
    Serial_SendByte(ch);
    return ch;
}

void Serial_Printf(char *format, ...){
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    Serial_SendString(String);
}
