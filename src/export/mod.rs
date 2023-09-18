#[macro_export]
macro_rules! hprintln {
    ($($tt:tt)*) => {
        #[cfg(debug_assertions)]
        {
            cortex_m_semihosting::hprintln!($($tt)*);
        }
    };
}
