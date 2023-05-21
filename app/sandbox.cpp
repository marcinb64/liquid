#include <BoardSelector.h>
#include <Gpio.h>
#include <Interrupts.h>
#include <Sys.h>
#include <Uart.h>

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

using namespace liquid;
using Board = liquid::ArduinoNano;

static Usart console {Board::makeUsart(0)};

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

    auto buttonToLed() -> void
    {
        int x = button.get();
        led.set(!x);
    }

    auto start() -> void { liquid::Sys::enableInterrupts(); }

    auto loop() -> void { echo(); }

private:
    Gpio led {Board::Gpio::BuiltInLed};
    Gpio button {Board::Gpio::D2};

    void setupSystem()
    {
        console.setupUart(19200L);
        liquid::installAsStdStreams(console);
    }

    void setupPinmux()
    {
        liquid::Gpio(Board::Gpio::BuiltInLed).asOutput();
        liquid::Gpio(Board::Gpio::D2).asInput(Gpio::Pullup::PullUp);
    }

    auto echo() -> void
    {
        static uint8_t rxbuf[64] = {0};

        int n = console.readLine(rxbuf, sizeof(rxbuf));
        console.tx('_');
        printf("%d: %s\n\r", n, rxbuf);
    }
};

/* -------------------------------------------------------------------------- */

int main()
{
    App app;
    app.start();
    while (true)
        app.loop();
}

void *operator new[](size_t size)
{
    return malloc(size);
}

void *operator new(size_t size)
{
    return malloc(size);
}

#include "avr/BoardIsrSelector.cpp"
