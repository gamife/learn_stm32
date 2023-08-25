#include "stm32f10x.h"
// #include "MySPI.h"
#include "MySPI_Hard.h"
#include "W25Q64.h"
#include "W25Q64_Ins.h"

void W25Q64_Init(void)
{ 
    // 软件模拟SPI
    // MySPI_Init();

    // 硬件SPI
    MySPI_Hard_Init();

}

/**
  * @brief  获取W25Q64的ID
  * @param  MID: 厂商ID
  * @param  DID: 设备ID
  * @retval None
  */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID){
    // 起始位
    MySPI_Start();
    // w25q64指令, 根据w25q64手册找对应指令, 以及其后续的参数意义
    MySPI_SwapByte(W25Q64_JEDEC_ID);

    // 这里主机读, 所以发送一个任意的字节, 这里发送0xFF
    *MID= MySPI_SwapByte(W25Q64_DUMMY_BYTE);
    *DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
    *DID <<= 8;
    *DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);

    MySPI_Stop();
}


/**
  * @brief  W25Q64写使能
  *         在写入闪存前, 要先写使能
  * @retval None
  */
void W25Q64_WriteEnable(void){
    MySPI_Start();
    MySPI_SwapByte(W25Q64_WRITE_ENABLE);
    MySPI_Stop();
}

/**
  * @brief  等待W25Q64忙结束
  *         W25Q64有一个256Byte的RAM, 用来缓存写入的数据, 真正落盘比较慢, 就处于Busy状态, 要等待Busy结束, 才可以写或者读下一次的数据
  *         Busy状态时, 除了 Read Status Register 和 Erase Suspend 指令外, 其他的都会被忽略, 包括读数据
  * @param  None
  * @retval None
  */
void W25Q64_WaitBusy(void){
    uint32_t Timeout;

    MySPI_Start();
    // 读W25Q64的状态寄存器1, 第0位是Busy的标志位
    MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
    Timeout = 100000;
    // 这条读状态寄存器的指令, 只要不结束SPI通信, 就会重复的发过来
    while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01){
        Timeout --;
        if (Timeout == 0){
            break;
        }
    };
    MySPI_Stop();
}

/**
  * @brief  写入W25Q64的某一页. 一页=256B=2^8, 所以页起始地址类似 xxxx00.
  *         W25Q64有一个256Byte的RAM, 用来缓存写入的数据, 256Byte也就是磁盘的一页, 如果一次SPI写数据超过256Byte, 后面的数据就会覆盖当页开头的数据.
  *         所以写入的数据不能跨页, 比如, 如果往0x0000FF写2个byte, 由于0x000100这个地址与0x0000FF不在同一页, 所以实际0x000100并没有写入数据, 而是写到了0x000001, 
  *         注意, 因为只写了2个byte, 0x000002等其他无关地址会保持不变.
  *         W25Q64每个数据位只能由1改写为0，不能由0改写为1(擦除操作可以), 所以写之前最好擦除一下(全部置1), 擦除的最小单位是扇区(4KB)
  * @param  Address: 写入的页起始地址, 不能跨页 W25Q64的地址最大: 0x7FFFFF = 2^23 = 8M
  * @param  DataArray: 数据数组
  * @param  DataLen: 数据数组的长度, 范围: 1-256
  * @retval None
  */
void W25Q64_PageProgram(uint32_t Address, uint8_t* DataArray, uint16_t DataLen){
    uint16_t i;
    // 写之前要写使能, 写失能会在SPI写结束后, 由硬件设置, 具体看手册 11.1.2 Write Enable Latch (WEL)
    W25Q64_WriteEnable();

    MySPI_Start();
    MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
    // uint32_t 强制 uint8_t, 高位截断
    // A23-A16
    MySPI_SwapByte(Address >> 16);
    // A15-A8
    MySPI_SwapByte(Address >> 8);
    // A7-A0
    MySPI_SwapByte(Address);

    for (i = 0; i < DataLen; i++){
        MySPI_SwapByte(DataArray[i]);
    }

    MySPI_Stop();

    // 等待实际落盘. 也可以放在函数开始, 但是放在函数开始, 那么在读的函数也要加上waitBusy, 否则读也可能会因为闪存Busy被忽略.
    W25Q64_WaitBusy();
}

/**
  * @brief  擦除W25Q64的扇区(4KB), 擦除的最小单位就是扇区
  *         4K=4*1024=2^12
  * @param  Address: 要擦除的扇区起始地址, W25Q64的地址最大: 0x7FFFFF = 2^23 = 8M, 扇区的起始地址都类似: xxx000 , 低三位都是0(3*4=12)
  *         对于 0x2FF000和0x2FF004, 其实擦除的都是同一个扇区.
  * @retval None
  */
void W25Q64_SectorErase(uint32_t Address){
    // 写之前要写使能, 写失能会在SPI写结束后, 由硬件设置
    W25Q64_WriteEnable();

    MySPI_Start();
    MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
    // uint32_t 强制 uint8_t, 高位截断
    // A23-A16
    MySPI_SwapByte(Address >> 16);
    // A15-A8
    MySPI_SwapByte(Address >> 8);
    // A7-A0
    MySPI_SwapByte(Address);
    MySPI_Stop();
    
    // 等待实际落盘. 也可以放在函数开始, 但是放在函数开始, 那么在读的函数也要加上waitBusy, 否则读也可能会因为闪存Busy被忽略.
    W25Q64_WaitBusy();
}

/**
  * @brief  从W25Q64读取数据.
  * @param  Address: 读取数据的起始地址
  * @param  DataArray: 数据数组
  * @param  DataLen: 数据数组的长度, 表示读多少字节
  * @retval None
  */
void W25Q64_ReadData(uint32_t Address, uint8_t* DataArray, uint32_t DataLen){
    uint32_t i;
    MySPI_Start();
    MySPI_SwapByte(W25Q64_READ_DATA);
    // uint32_t 强制 uint8_t, 高位截断
    // A23-A16
    MySPI_SwapByte(Address >> 16);
    // A15-A8
    MySPI_SwapByte(Address >> 8);
    // A7-A0
    MySPI_SwapByte(Address);
    for (i = 0; i < DataLen; i++){
       DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
    }
    MySPI_Stop();
}
