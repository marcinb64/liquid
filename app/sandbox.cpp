#include <BoardSelector.h>
// #include <Gpio.h>
#include <Interrupts.h>
#include <Sys.h>
#include <Uart.h>

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

    auto start() -> void { liquid::Sys::enableInterrupts(); }

    auto loop() -> void
    {
        if (trigger) {
            trigger = 0;
            printf(".");
        }

        // echo();
    }

private:
    Gpio  led = Board::makeGpio(Board::Gpio::BuiltInLed);
    Gpio  button = Board::makeGpio(Board::Gpio::D2);
    
    volatile int trigger = 0;

    void setupSystem()
    {
        console.setupUart(19200L);
        liquid::installAsStdStreams(console);
    }

    void setupPinmux()
    {
        led.asOutput();
        led.setLow();
        button.asInput(Gpio::Pullup::PullUp);
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

int main()
{
    App app;
    app.start();
    while (true)
        app.loop();

    return 0;
}

#include "avr/BoardIsrSelector.cpp"
