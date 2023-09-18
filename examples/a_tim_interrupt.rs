#![no_std]
#![no_main]

use app::*;

#[cfg(not(debug_assertions))]
use panic_halt as _;
#[cfg(debug_assertions)]
use panic_semihosting as _; // logs messages to the host stderr; requires a debugger // you can put a breakpoint on `rust_begin_unwind` to catch panics
                            // use panic_abort as _; // requires nightly
                            // use panic_itm as _; // logs messages over ITM; requires ITM support

use stm32f1xx_hal::{
    device::{TIM1, TIM2},
    gpio::{gpioa::PA1, Output, PushPull},
    pac::interrupt,
    time::Hertz,
    timer::{self, CounterHz},
};

use core::{
    borrow::BorrowMut,
    cell::RefCell,
    fmt::Write,
    ops::DerefMut,
    ptr,
    sync::atomic::{AtomicUsize, Ordering},
};
use cortex_m::{interrupt::Mutex, peripheral::syst::SystClkSource};
use cortex_m_rt::{entry, exception, ExceptionFrame};
use cortex_m_semihosting::hprintln;
use fugit::{Duration, ExtU32};
use stm32f1xx_hal::timer::Timer;
use stm32f1xx_hal::{pac, prelude::*};

const AIRCR_VECTKEY_MASK: u32 = 0x05FA0000;
const NVIC_PRIORITY_GROUP_0: u32 = 0x700; // 0 bits for pre-emption priority, 4 bits for subpriority
const NVIC_PRIORITY_GROUP_1: u32 = 0x600; // 1 bits for pre-emption priority, 3 bits for subpriority
const NVIC_PRIORITY_GROUP_2: u32 = 0x500; // 2 bits for pre-emption priority, 2 bits for subpriority
const NVIC_PRIORITY_GROUP_3: u32 = 0x400; // 3 bits for pre-emption priority, 1 bits for subpriority
const NVIC_PRIORITY_GROUP_4: u32 = 0x300; // 4 bits for pre-emption priority, 0 bits for subpriority

// static mut COUNTER: u32 = 0;
static COUNTER: AtomicUsize = AtomicUsize::new(0);

static LED: Mutex<RefCell<Option<PA1<Output<PushPull>>>>> = Mutex::new(RefCell::new(None));

static TIM: Mutex<RefCell<Option<CounterHz<TIM2>>>> = Mutex::new(RefCell::new(None));

#[entry]
fn main() -> ! {
    hprintln!("Hello, world!");

    let p = pac::Peripherals::take().unwrap();

    let mut flash = p.FLASH.constrain();
    let mut rcc = p.RCC.constrain();

    let clocks = system::rcc_init::set_sys_clock_to72(rcc.cfgr, &mut flash.acr);
    hprintln!("clk: {:?} Hz", clocks.pclk1_tim().to_Hz());

    let mut gpioa = p.GPIOA.split();
    let mut PA1 = gpioa.pa1.into_push_pull_output(&mut gpioa.crl);
    PA1.set_low();

    cortex_m::interrupt::free(|cs| LED.borrow(cs).replace(Some(PA1)));

    let mut cp = cortex_m::Peripherals::take().unwrap();
    unsafe {
        // AIRCR_VECTKEY_MASK 这个值是固定要写的. 手册里On writes, write 0x5FA to VECTKEY, otherwise the write is ignored.
        cp.SCB.aircr.modify(|mut raw| {
            raw |= AIRCR_VECTKEY_MASK | NVIC_PRIORITY_GROUP_2;
            raw
        });
        // SCB_AIRCR_PRIGROUP 设置NVIC优先级分组, 优先级有4个bit, 但是优先级有两种(1.嵌套中断,也就是中断打断中断 2.中断排队), 所以4个bit中几个bit作为嵌套中断优先级就是由这里设置的
        // NVIC_IPRx 用来设置中断通道的优先级, 4个bit有效, 但是参数是u8, 所以其实只有[7:4] 是有效的
        // NVIC_ISERx 使能对应的中断通道
        cp.NVIC.set_priority(interrupt::TIM2, 0b1111 << 4);
        cortex_m::peripheral::NVIC::unmask(interrupt::TIM2);
    }

    // TIM的频率为72MHz, 计数器和预分频器都为16位, 所以有个最值 72MHz/(2^16+1)/(2^16+1)~=0.01676Hz
    // 1.RCC时钟启动TIM1
    // 2.配置时基单元
    let timeout = 2.Hz();
    let mut tim = p.TIM2.counter_hz(&clocks);
    tim.start(timeout).unwrap();
    // let mut tim = Timer::<TIM2>::tim2(p.TIM2, timeout, clocks, &mut rcc.apb1);

    // 使能计数器的更新中断
    tim.listen(stm32f1xx_hal::timer::Event::Update);
    // 启动中断后, 这里再次start初始化会panic
    // tim.start(timeout);

    cortex_m::interrupt::free(|cs| TIM.borrow(cs).replace(Some(tim)));

    // 查看一下TIM的有关寄存器
    // let a:  &pac::tim2::RegisterBlock =   unsafe { &*  (1073741824 as *const _) };
    // hprintln!("arr: {:#x}", a.arr.read().arr().bits());
    // hprintln!("psc: {:#x}", a.psc.read().psc().bits());

    loop {
        // let count = unsafe { COUNTER };
        let count = COUNTER.load(Ordering::Relaxed);
        hprintln!("interupt: {}", count);

        cortex_m::interrupt::free(|cs| {
            let mut led = LED.borrow(cs).borrow_mut();
            led.as_mut().unwrap().toggle();
        });

        // 如果不开启中断, 可以等待 TIMx_SR_UIF：更新中断标记 置一
        // while tim.wait().is_err(){}

        // 浅睡眠
        cortex_m::asm::wfi();
    }
}

#[interrupt]
fn TIM2() {
    COUNTER.fetch_add(1, Ordering::Relaxed);
    // unsafe { COUNTER += 1; }

    cortex_m::interrupt::free(|cs| {
        // let mut led = MY_GPIO.borrow(cs).borrow_mut();
        // led.as_mut().unwrap().toggle();

        let mut tim = TIM.borrow(cs).borrow_mut();
        // 清除中断标志位
        let _ = tim.as_mut().unwrap().clear_interrupt(timer::Event::Update);
    });
}
