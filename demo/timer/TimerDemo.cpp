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
    static constexpr auto outputPin = Board::Gpio::D9;

    static void setupPinmux()
    {
        Board::makeGpio(led).asOutput();
        Board::makeGpio(outputPin).asOutput();
    }
};

static auto led = Board::makeGpio(BoardConfig::led);

// ----------------------------------------------------------------------------

/**
 * Output square wave using a hardware timer
 *
 * Calculates the parameters at run time.
 * Can work with any SquareWave implementation, using different hardware timers.
 */
auto outputSquareWave(SquareWave &out, float f) -> void
{
    out.configure(F_CPU, f);
}

auto squareWaveWrapperDemo()
{
    auto squareWave1 = Board::makeSquareWave(BoardConfig::outputPin);
    outputSquareWave(squareWave1, 300);
}

/**
 * Output square wave using a hardware timer - optimized variant
 *
 * If all inputs are know at compile time, it offers compile-time frequency
 * calculation and range checking.
 */
auto squareWaveDirectDemo()
{
    using CTC = AvrTimer16::CTCMode;

    constexpr auto cfg1 = CTC::configureSquareWave(F_CPU, 99300400);
    static_assert(cfg1.isValid == false);

    constexpr auto cfg2 = CTC::configureSquareWave(F_CPU, 0.0001f);
    static_assert(cfg2.isValid == false);

    constexpr auto cfg3 = CTC::configureSquareWave(F_CPU, 300);
    static_assert(cfg3.isValid);

    constexpr auto t1 = Board::makeTimer16(Timer16::Timer1);
    t1.apply(cfg3);
}

// -----------------------------------------------------------------------------

/**
 * Output PWM using a hardware timer
 *
 * Calculates the parameters at run time.
 * Can work with any SquareWave implementation, using different hardware timers.
 */
auto outputPwm(Pwm &out, [[maybe_unused]] unsigned long f0, [[maybe_unused]] unsigned long f1,
               float v) -> void
{
    const auto success = out.configure(F_CPU, f0, f1);
    if (success)
        out.setDutyCycle(v);
}

auto pwmWrapperDemo()
{
    auto pwm1 = Board::makePwm(BoardConfig::outputPin);
    outputPwm(pwm1, 10000, 40000, 0.25f);
}

auto pwmDirectDemo()
{
    constexpr auto t1 = Board::makeTimer16(Timer16::Timer1);
    constexpr auto OutA = CompareOutputChannel::ChannelA;
    using Pwm = AvrTimer16::FastPwmMode;

    static_assert(Pwm::findFrequency(F_CPU, 900, 1200) == 0);       // frequency range too narrow
    static_assert(Pwm::findFrequency(F_CPU, 1, 10) == 0);           // frequency range too low
    static_assert(Pwm::findFrequency(F_CPU, 100000, 9100200) == 0); // frequency range too high
    static_assert(Pwm::findFrequency(F_CPU, 10000, 40000) != 0);    // accepteable range

    constexpr auto config1 = Pwm::configure(OutA, F_CPU, 10000, 40000);
    static_assert(config1.isValid);
    t1.apply(config1);

    constexpr auto config2 = Pwm::setDutyCycle(0.25f, OutA);
    static_assert(config2.isValid);
    t1.apply(config2);
}

// -----------------------------------------------------------------------------

static auto periodicInterrupt(void *) -> void
{
    Board::makeGpio(BoardConfig::outputPin).toggle();
}

auto periodicInterruptWrapperDemo() -> void
{
    auto timer = TimerImpl16(Board::makeTimer16(Timer16::Timer1));

    // freq too low
    timer.enablePeriodicInterrupt(F_CPU, 0.001f, IrqHandler {periodicInterrupt, nullptr});

    // freq too high
    timer.enablePeriodicInterrupt(F_CPU, 99000000.0f, IrqHandler {periodicInterrupt, nullptr});

    // toggle the LED every 1s
    timer.enablePeriodicInterrupt(F_CPU, 1.0f, IrqHandler {periodicInterrupt, nullptr});
}

auto periodicInterruptDirectDemo() -> void
{
    constexpr auto t1 = Board::makeTimer16(Timer16::Timer1);

    // freq too low
    constexpr auto cfg1 = AvrTimer16::CTCMode::configurePeriodicInterrupt(
        F_CPU, 0.001f, IrqHandler {periodicInterrupt, nullptr});
    static_assert(cfg1.isValid == false);

    // freq too high
    constexpr auto cfg2 = AvrTimer16::CTCMode::configurePeriodicInterrupt(
        F_CPU, 99000000.0f, IrqHandler {periodicInterrupt, nullptr});
    static_assert(cfg2.isValid == false);

    // toggle the LED every 1s
    constexpr auto cfg3 = AvrTimer16::CTCMode::configurePeriodicInterrupt(
        F_CPU, 1.0f, IrqHandler {periodicInterrupt, nullptr});
    static_assert(cfg3.isValid == true);

    t1.apply(cfg3);
}

/* -------------------------------------------------------------------------- */

#include <SysTimer.h>

auto appMain() -> void
{
    BoardConfig::setupPinmux();

    SysTimer sysTimer;
    AvrTimer8 timer0 = Board::makeTimer8(Timer8Id::Timer0);
    sysTimer.setupWith(timer0, F_CPU, 1000);
    
    // constexpr auto demo = squareWaveWrapperDemo;
    // constexpr auto demo = squareWaveDirectDemo;
    constexpr auto demo = pwmDirectDemo;
    // constexpr auto demo = pwmWrapperDemo;
    // constexpr auto demo = periodicInterruptDirectDemo;
    // constexpr auto demo = periodicInterruptWrapperDemo;

    demo();
    
    Sys::enableInterrupts();
    while (true) {
        Board::makeGpio(BoardConfig::led).toggle();
        _delay_ms(2000);
        printf("t=%lu\n\r", sysTimer.getTime());
    }
        
}
