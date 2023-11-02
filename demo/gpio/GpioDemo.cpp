#include "app.h"
#include <avr/BoardSelector.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <util/delay.h>

using namespace liquid;

constexpr static auto DEBUG = false;
constexpr static auto HEARTBEAT = false;

struct BoardConfig {
    static constexpr auto led = Board::Gpio::BuiltInLed;
    static constexpr auto button1Pin = Board::Gpio::D11;
    static constexpr auto button2Pin = Board::Gpio::D12;

    static void setupPinmux()
    {
        Board::makeGpio(led).asOutput();
        Board::makeGpio(button1Pin).asInput(Gpio::Pullup::PullUp);
        Board::makeGpio(button2Pin).asInput(Gpio::Pullup::PullUp);
    }
};

static auto button1 = Board::makeGpio(BoardConfig::button1Pin);
static auto button2 = Board::makeGpio(BoardConfig::button2Pin);
static auto led = Board::makeGpio(BoardConfig::led);

/* -------------------------------------------------------------------------- */

auto pulse(Gpio &out)
{
    out.setHigh();
    _delay_ms(4000);
    out.setLow();
}

auto pulseLedIfButton1Pressed()
{
    auto pressed = !button1.get();

    if (pressed) {
        pulse(led);
        while (!button1.get())
            ;
    }
}

static auto buttonInterrupt(void *arg) -> void
{
    Gpio *out = reinterpret_cast<Gpio*>(arg);
    if (button2.get() == false)
        out->toggle();
}

auto setupButton2Interrupt() -> void
{
    constexpr auto irq = BoardConfig::button2Pin.getIrq();
    static_assert(isValidIrq(irq));
    
    installIrqHandler(irq, { buttonInterrupt, &led });
    button2.enableInterrupt();
}

/* -------------------------------------------------------------------------- */

auto appMain() -> void
{
    BoardConfig::setupPinmux();
    led.setLow();
    setupButton2Interrupt();

    Sys::enableInterrupts();

    while (true) {
        pulseLedIfButton1Pressed();
    }
}
