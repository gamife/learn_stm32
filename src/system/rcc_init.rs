use stm32f1xx_hal::{flash::ACR, rcc::CFGR, time::U32Ext};

pub fn set_sys_clock_to72(rcc_cfgr: CFGR, flash_arc: &mut ACR) {
    rcc_cfgr
        // 使用高速外部时钟HSE, 作为PLL的输入
        .use_hse(8.mhz())
        // PLL时钟作为 SYSCLK 系统时钟, (PLL最大9倍)
        .sysclk(72.mhz())
        // SYSCLK 分频得到HCLK (/1,2...512)
        .hclk(72.mhz())
        // APB2 最大72Mhz, 分频是有限制的
        .pclk2(72.mhz())
        // APB1 最大36Mhz, 分频是有限制的
        .pclk1(36.mhz())
        .freeze(flash_arc);
}
