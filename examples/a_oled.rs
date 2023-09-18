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
// use cortex_m_semihosting::hprintln;
use embedded_graphics::{
    pixelcolor::BinaryColor,
    prelude::*,
    primitives::{Circle, PrimitiveStyleBuilder, Rectangle, Triangle},
};
use fugit::{Duration, ExtU32};
use ssd1306::{prelude::*, I2CDisplayInterface, Ssd1306};
use stm32f1xx_hal::timer::Timer;
use stm32f1xx_hal::{
    i2c::{BlockingI2c, DutyCycle, Mode},
    stm32,
};
use stm32f1xx_hal::{pac, prelude::*};

#[entry]
fn main() -> ! {
    hprintln!("Hello, world!");

    let p = pac::Peripherals::take().unwrap();

    let mut flash = p.FLASH.constrain();
    let mut rcc = p.RCC.constrain();

    let clocks = system::rcc_init::set_sys_clock_to72(rcc.cfgr, &mut flash.acr);
    hprintln!("clk: {:?} Hz", clocks.pclk1_tim().to_Hz());

    let mut afio = p.AFIO.constrain();
    let mut gpiob = p.GPIOB.split();
    let scl = gpiob.pb8.into_alternate_open_drain(&mut gpiob.crh);
    let sda = gpiob.pb9.into_alternate_open_drain(&mut gpiob.crh);

    let mut gpioa = p.GPIOA.split();
    let mut PA1 = gpioa.pa1.into_push_pull_output(&mut gpioa.crl);
    PA1.set_low();

    let i2c = BlockingI2c::i2c1(
        p.I2C1,
        (scl, sda),
        &mut afio.mapr,
        Mode::Fast {
            frequency: 400_000.Hz(),
            duty_cycle: DutyCycle::Ratio2to1,
        },
        clocks,
        1000,
        10,
        1000,
        1000,
    );

    let interface = I2CDisplayInterface::new(i2c);
    let mut display = Ssd1306::new(interface, DisplaySize128x64, DisplayRotation::Rotate0)
        .into_buffered_graphics_mode();
    display.init().unwrap();

    let yoffset = 8;

    let style = PrimitiveStyleBuilder::new()
        .stroke_width(1)
        .stroke_color(BinaryColor::On)
        .build();

    // 清空屏幕
    // display.clear(BinaryColor::Off);
    // display.flush().unwrap();
    // loop{}

    // screen outline
    // default display size is 128x64 if you don't pass a _DisplaySize_
    // enum to the _Builder_ struct
    Rectangle::new(Point::new(0, 0), Size::new(127, 63))
        .into_styled(style)
        .draw(&mut display)
        .unwrap();

    // 这个画三角形的, 用openocd会报错, 但是直接.bin文件下载到板子上没问题.
    // triangle
    // Triangle::new(
    //     Point::new(16, 16 + yoffset),
    //     Point::new(16 + 16, 16 + yoffset),
    //     Point::new(16 + 8, yoffset),
    // )
    // .into_styled(style)
    // .draw(&mut display)
    // .unwrap();

    // square
    Rectangle::new(Point::new(52, yoffset), Size::new_equal(16))
        .into_styled(style)
        .draw(&mut display)
        .unwrap();

    // circle
    Circle::new(Point::new(88, yoffset), 16)
        .into_styled(style)
        .draw(&mut display)
        .unwrap();

    display.flush().unwrap();

    loop {}
}
