#include <BoardSelector.h>
#include <Interrupts.h>
#include <Sys.h>
#include <Uart.h>
#include "Pwm.h"
#include "SysTimer.h"

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>


using namespace liquid;
using Board = liquid::ArduinoNano;

static Usart console = Board::makeUsart(0);

class App
{
public:
    App()
    {
        setupPinmux();
        setupSystem();

        installGpioIsr(callMethod<App, &App::buttonToLed>, this);
        button.enableInterrupt();
    }

    auto start() -> void
    {
        liquid::Sys::enableInterrupts();
    }

    auto loop() -> void
    {
        static volatile bool a = false;
        _delay_ms(500);
        led.set(a);
        a = !a;

        // unsigned int value = adc.readRaw(1);
        // printf("ADC raw: %d\r\n", value);
        // pwm.set(value / 1023.0f);
        
        while (adc.readRaw(1) < 500);

        printf("Start\r\n");
        auto t0 = SysTimer::getTime();
        while (SysTimer::getTime() < (t0 + 10000));
        printf("Done\r\n");
        
    }

private:
    Gpio       led = Board::makeGpio(Board::Gpio::BuiltInLed);
    Gpio       button = Board::makeGpio(Board::Gpio::D2);
    Adc        adc = Board::makeAdc();
    Gpio       out = Board::makeGpio(Board::Gpio::D9);
    Pwm        pwm = Board::makePwmD9();

    volatile int trigger = 0;

    void setupSystem()
    {
        console.setupUart(19200L);
        liquid::installAsStdStreams(console);
        Board::enableSysTimer();
    }

    void setupPinmux()
    {
        led.asOutput();
        led.setLow();
        button.asInput(Gpio::Pullup::PullUp);
        out.asOutput();
    }

    auto echo() -> void
    {
        static uint8_t rxbuf[64] = {0};

        int n = console.readLine(rxbuf, sizeof(rxbuf));
        console.tx('_');
        printf("%d: %s\n\r", n, rxbuf);
    }

    auto buttonToLed() -> void
    {
        int x = button.get();
        led.set(!x);

        trigger = 1;
    }
};

/* -------------------------------------------------------------------------- */

void crashHandler()
{
    Gpio led = Board::makeGpio(Board::Gpio::BuiltInLed);
    led.asOutput();

    while (1) {
        led.setHigh();
        _delay_ms(100);
        led.setLow();
        _delay_ms(100);
    }
}

int main()
{
    atexit(crashHandler);

    App app;
    app.start();
    while (true)
        app.loop();

    return 0;
}

#include "avr/BoardIsrSelector.cpp"
