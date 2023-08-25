#include "stm32f10x.h"
#include "MyI2C.h"
#include "MyI2C_Hard.h"
#include "MPU6050.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS  0xD0

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data){
    // MyI2C_Start();

    // MyI2C_SendByte(MPU6050_ADDRESS);
    // // 可以判断有没有找到陀螺仪
    // MyI2C_ReciveAck();

    // MyI2C_SendByte(RegAddress);
    // // 可以判断有没有找到陀螺仪
    // MyI2C_ReciveAck();

    // MyI2C_SendByte(Data);
    // // 可以判断有没有找到陀螺仪
    // MyI2C_ReciveAck();

    // MyI2C_Stop();

    // 硬件时序, 其中的事件可查看stm手册
    I2C_GenerateSTART(I2C2, ENABLE);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
    I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
    I2C_SendData(I2C2, RegAddress);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);
    I2C_SendData(I2C2, Data);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
    I2C_GenerateSTOP(I2C2, ENABLE);

}

uint8_t MPU6050_ReadReg(uint8_t RegAddress){
    uint8_t data;

    // MyI2C_Start();
    // // 主机虽然需要读, 但是指定addr读, 最后一位仍然是写标志位(0)
    // // 这样就可以让 从机指向对应内存地址
    // MyI2C_SendByte(MPU6050_ADDRESS);
    // MyI2C_ReciveAck();
    // MyI2C_SendByte(RegAddress);
    // MyI2C_ReciveAck();

    // // 重复起始条件, 这次指定设备需要置 读标志位
    // MyI2C_Start();
    // MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
    // MyI2C_ReciveAck();
    // data = MyI2C_ReciveByte();
    // // 接收应答给1, 主机后续不再接收了.
    // MyI2C_SendAck(1);

    // MyI2C_Stop();

    I2C_GenerateSTART(I2C2, ENABLE);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

    I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);

    I2C_SendData(I2C2, RegAddress);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);

    // 重复start
    I2C_GenerateSTART(I2C2, ENABLE);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

    I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);
    // 注意这里主机地址的接收和发送完成事件, 是不一样的
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);

    // 在接收数据前, 需要设置接收应答
    I2C_AcknowledgeConfig(I2C2,DISABLE);
    // 同样, 也要提前设置stop 
    I2C_GenerateSTOP(I2C2, ENABLE);
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
    
    data = I2C_ReceiveData(I2C2);

    // 提前设置下一次的ack
    I2C_AcknowledgeConfig(I2C2,ENABLE);

    return data;
}

void MPU6050_Init(void){
    // 1. 软件I2C
    // MyI2C_Init();

    // 2. 硬件I2C
    MyI2C_Hard_Init();

    // 配置电源管理寄存器1, 取消休眠模式, 选择陀螺仪时钟
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    // 配置电源管理寄存器2  六个轴都不待机
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    // 采样分频
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    // 配置无外部帧同步, 低三位数字滤波
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    // 陀螺仪自检及量程选择
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    // 加速计自检,量程选择, 正负16g的加速度测量范围
    // MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);
}

uint8_t MPU6050_GetID(void){
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(MPU6050_DataStruct* data){
    uint8_t DataH, DataL;

    // 这里使用的都是指定地址的读取, 也可以尝试使用读连续地址, 这样只要指定一次起始地址
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    data->Accx = (DataH << 8) | DataL; 

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    data->Accy = (DataH << 8) | DataL; 

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    data->Accz = (DataH << 8) | DataL; 

    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    data->Gyrox = (DataH << 8) | DataL; 

    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    data->Gyroy = (DataH << 8) | DataL; 

    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    data->Gyroz = (DataH << 8) | DataL; 

    return; 
}

