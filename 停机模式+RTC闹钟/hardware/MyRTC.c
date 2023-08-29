#include "stm32f10x.h"
#include "MyRTC.h"
#include <time.h>

uint16_t MyRTC_Time[] = {2023,1,1,23,55,11};

void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	// 电源控制寄存器(PWR_CR)的DBP位来使能对后备寄存器和RTC的访问。
	PWR_BackupAccessCmd(ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR9) == 0x1234){
        // 如果备用寄存器没有掉电, 说明备用电源维持着RTC, 就不需要初始化RTC的配置了, 等待一下RTC的硬件初始化即可
    	RTC_WaitForSynchro();
	    RTC_WaitForLastTask();   
        return;
    }else{
        // 重新写入一下BKP
        BKP_WriteBackupRegister(BKP_DR9, 0x1234);
    }


	// 外部低速振荡器使能
	RCC_LSEConfig(RCC_LSE_ON);
	// 等待 外部低速LSE就绪
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);

	// 使用LSE作为 RTC的时钟输入
    // 也可以使用LSI内部震荡器, 但是其频率为 40Khz
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	// 使能RTC时钟
	RCC_RTCCLKCmd(ENABLE);

	// 若在读取RTC寄存器时，RTC的APB1接口曾经处于禁止状态，则软件首先必须等待RTC_CRL寄存器中的RSF位(寄存器同步标志)被硬件置’1’
	RTC_WaitForSynchro();
	// 对RTC任何寄存器的写操作，都必须在前一次写操作结束后进行。可以通过查询RTC_CR寄存器中的RTOFF状态位，判断RTC寄存器是否处于更新中。仅当RTOFF状态位是’1’时，才可以写入RTC寄存器
	RTC_WaitForLastTask();

	// 板子上焊接的晶振=32.768KHz, 所以这里预分频器给32768, 刚好可以给RTC输入一个1Hz的时钟
	// 另外, 必须设置RTC_CRL寄存器中的CNF位，使RTC进入配置模式后, 才能写才能写入RTC_PRL、RTC_CNT、RTC_ALR寄存器, 库函数已经写好了.
	RTC_SetPrescaler(32768-1);
	// 等待RTC任何寄存器的写操作完成, 也就是上一步设置的预分频
	RTC_WaitForLastTask();

	// 设置初试时间
	RTC_SetCounter(1692968221);
	RTC_WaitForLastTask();
}

void MyRTC_SetTime(void){
    struct tm datetime;

    datetime.tm_year = MyRTC_Time[0] - 1900;
    datetime.tm_mon= MyRTC_Time[1] - 1;
    datetime.tm_mday = MyRTC_Time[2];
    datetime.tm_hour = MyRTC_Time[3];
    datetime.tm_min = MyRTC_Time[4];
    datetime.tm_sec = MyRTC_Time[5];

    time_t time_cnt = mktime(&datetime);    

    RTC_SetCounter(time_cnt);
    // 等待RTC任何寄存器的写操作完成
	RTC_WaitForLastTask();
}

struct tm *MyRTC_GetTime(void){
    time_t time_cnt;
    time_cnt = RTC_GetCounter();
    return localtime(&time_cnt);
}
