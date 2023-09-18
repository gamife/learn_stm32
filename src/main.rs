#![no_std]
#![no_main]

use app::system;
#[cfg(not(debug_assertions))]
use panic_halt as _;
#[cfg(debug_assertions)]
use panic_semihosting as _; // logs messages to the host stderr; requires a debugger // you can put a breakpoint on `rust_begin_unwind` to catch panics
                            // use panic_abort as _; // requires nightly
                            // use panic_itm as _; // logs messages over ITM; requires ITM support

use stm32f1xx_hal::pac::interrupt;

use core::fmt::Write;
use cortex_m::peripheral::syst::SystClkSource;
use cortex_m_rt::{entry, exception, ExceptionFrame};
use cortex_m_semihosting::{
    hio::{self, HostStream},
    hprintln,
};
use stm32f1xx_hal::{pac, prelude::*};

#[entry]
fn main() -> ! {
    // cortex_m::asm::nop(); // To not have main optimize to abort in release mode, remove when you add code
    hprintln!("Hello, world!");

    let p = pac::Peripherals::take().unwrap();

    let mut flash = p.FLASH.constrain();
    let rcc = p.RCC.constrain();

    system::rcc_init::set_sys_clock_to72(rcc.cfgr, &mut flash.acr);

    let mut gpioa = p.GPIOA.split();
    gpioa.pa1.into_push_pull_output(&mut gpioa.crl).set_low();

    let cp = cortex_m::Peripherals::take().unwrap();
    let mut syst = cp.SYST;

    // 这个Cortex系统时钟(内核的), 是由 HCLK/8=72MHz/8=9MHz
    syst.set_clock_source(SystClkSource::External);
    // 重装寄存器只有24bit, 最大值是16_777_215
    // 这里填9M, 每一秒产生一个SysTick中断.
    syst.set_reload(9_000_000);
    syst.clear_current();
    syst.enable_counter();
    syst.enable_interrupt();

    // 读一个不存在的地址, 触发HardFault中断
    // unsafe {
    //     ptr::read_volatile(0x3FFF_FFFE as *const u32);
    // }
    loop {}
}

#[exception]
fn SysTick() {
    static mut COUNT: u32 = 0;
    static mut STDOUT: Option<HostStream> = None;

    *COUNT += 1;

    // Lazy initialization
    if STDOUT.is_none() {
        *STDOUT = hio::hstdout().ok();
    }

    if let Some(hstdout) = STDOUT.as_mut() {
        write!(hstdout, "{}", *COUNT).ok();
    }
}

#[exception]
unsafe fn HardFault(ef: &ExceptionFrame) -> ! {
    if let Ok(mut hstdout) = hio::hstdout() {
        writeln!(hstdout, "{:#?}", ef).ok();
    }

    loop {}
}

#[interrupt]
fn TIM2() {
    static mut COUNT: u32 = 0;

    // `COUNT` has type `&mut u32` and it's safe to use
    *COUNT += 1;
}
