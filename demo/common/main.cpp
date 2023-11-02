#include "app.h"

#include <Sys.h>
#include <avr/BoardSelector.h>

#include <util/delay.h>

using namespace liquid;

constexpr static auto uartBaudRate = 19200L;

static Usart console = Board::makeUsart(0);

/* -------------------------------------------------------------------------- */

// Blink the builtin LED to indicate a fatal error
void crashHandler()
{
    Sys::disableInterrupts();
    Gpio led = Board::makeGpio(Board::Gpio::BuiltInLed);
    led.asOutput();

    while (1) {
        led.setHigh();
        _delay_ms(100);
        led.setLow();
        _delay_ms(100);
    }
}

extern "C" void __cxa_pure_virtual()
{
    crashHandler();
}

void setupSystem()
{
    console.setupUart(F_CPU, uartBaudRate);
    liquid::installAsStdStreams(console);
}

int main()
{
    atexit(crashHandler);
    setupSystem();
    appMain();

    return 0;
}

#include "avr/BoardIsrSelector.cpp"
