#![no_main]
#![no_std]

use cortex_m_rt::entry;
use daisy::hal::adc;
use embedded_graphics::pixelcolor::BinaryColor;
use embedded_graphics::primitives::Circle;
use embedded_graphics::primitives::PrimitiveStyle;
use embedded_graphics::primitives::Rectangle;
use panic_semihosting as _;
use daisy::hal::delay::Delay;
use daisy::hal::prelude::*;
use stm32h7xx_hal::{self as hal, spi};

use embedded_graphics::prelude::*;
use ssd1306::prelude::*;
use ssd1306::Ssd1306;
use stm32h7xx_hal::delay::DelayFromCountDownTimer;

#[entry]
fn main() -> ! {
    let cp = cortex_m::Peripherals::take().unwrap();
    let dp = daisy::pac::Peripherals::take().unwrap();
    let board = daisy::Board::take().unwrap();

    let ccdr = daisy::board_freeze_clocks!(board, dp);
    let pins = daisy::board_split_gpios!(board, ccdr, dp);
    let mut led_user = daisy::board_split_leds!(pins).USER;

    let one_second = ccdr.clocks.sys_ck().to_Hz();

    // ADC 1
    let mut delay = Delay::new(cp.SYST, ccdr.clocks);
    let mut adc1 = adc::Adc::adc1(dp.ADC1, 4.MHz(), &mut delay, ccdr.peripheral.ADC12, &ccdr.clocks).enable();
    adc1.set_resolution(adc::Resolution::SixteenBit);
    // Select PIN to use for ADC
    let mut adc1_chan = pins.GPIO.PIN_22.into_analog();

    // LED
    let mut pin_4 = pins.GPIO.PIN_3.into_push_pull_output();
    
    let mut display = {
        // SPI
        let (clk, cs, din, mut res, dc) = (pins.GPIO.PIN_8.into_alternate(),
                                                    pins.GPIO.PIN_7.into_push_pull_output(), 
                                                    pins.GPIO.PIN_10.into_alternate(), 
                                                    pins.GPIO.PIN_30.into_push_pull_output(),
                                                    pins.GPIO.PIN_9.into_push_pull_output()
                                                    );
        let spi = dp.SPI1.spi((clk, spi::NoMiso, din), spi::MODE_0, 3.MHz(), ccdr.peripheral.SPI1, &ccdr.clocks, );
        let interface = display_interface_spi::SPIInterface::new(spi, dc, cs);
        let mut dispaly = Ssd1306::new(interface, DisplaySize128x64, DisplayRotation::Rotate0).into_buffered_graphics_mode();
        let mut delay = DelayFromCountDownTimer::new(dp.TIM2.timer(100.Hz(), ccdr.peripheral.TIM2, &ccdr.clocks));
        dispaly.reset(&mut res, &mut delay).unwrap();
        dispaly.init().unwrap();
        dispaly
    };


    // - main loop ------------------------------------------------------------
    loop {
        Rectangle::new(Point::new(0, 0), Size::new(127, 63)).into_styled(PrimitiveStyle::with_fill(BinaryColor::Off)).draw(&mut display).unwrap();
        Circle::new(Point::new(39, 7), 51).into_styled(PrimitiveStyle::with_fill(BinaryColor::On)).draw(&mut display).unwrap();
        display.flush().unwrap();
        cortex_m::asm::delay(one_second * 4);
        pin_4.toggle();
    }
}
