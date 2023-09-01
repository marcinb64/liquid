#include "app.h"
#include <avr/BoardSelector.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <util/delay.h>

using namespace liquid;
using Board = liquid::ArduinoMega;

constexpr static auto DEBUG = false;
constexpr static auto HEARTBEAT = false;

struct BoardConfig {
    static constexpr auto led = Board::Gpio::BuiltInLed;
    static constexpr auto squareWavePin = Board::Gpio::D5;
    static constexpr auto pwmPin = Board::Gpio::D12;

    static void setupPinmux()
    {
        Board::makeGpio(led).asOutput();
        Board::makeGpio(squareWavePin).asOutput();
        Board::makeGpio(pwmPin).asOutput();
    }
};

static auto squareWave1 = Board::makeSquareWave(BoardConfig::squareWavePin);
static auto pwm1 = Board::makePwm(BoardConfig::pwmPin);
static auto timer = TimerImpl16(Board::makeTimer16(Timer16::Timer5));

static auto led = Board::makeGpio(BoardConfig::led);

/* -------------------------------------------------------------------------- */

/**
 * Output square wave using a hardware timer
 *
 * Calculates the parameters at run time.
 * Can work with any SquareWave implementation, using different hardware timers.
 */
auto outputSquareWave([[maybe_unused]] SquareWave &out, [[maybe_unused]] float f) -> void
{
    assert(out.configure(F_CPU, f));
}

/**
 * Output square wave using a hardware timer - optimized variant
 *
 * If all inputs are know at compile time, it offers compile-time frequency
 * calculation and range checking.
 */
auto outputSquareWaveOptimized() -> void
{
    using CTC = AvrTimer16::CTCMode;

    constexpr auto cfg1 = CTC::configureSquareWave(F_CPU, 99300400);
    static_assert(cfg1.isValid == false);

    constexpr auto cfg2 = CTC::configureSquareWave(F_CPU, 0.0001f);
    static_assert(cfg2.isValid == false);

    constexpr auto cfg3 = CTC::configureSquareWave(F_CPU, 300);
    static_assert(cfg3.isValid);

    constexpr auto t3 = Board::makeTimer16(Timer16::Timer3);
    t3.apply(cfg3);
}

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
    assert(success);
    out.setDutyCycle(v);
}

auto outputPwmOptimized()
{
    constexpr auto t1 = Board::makeTimer16(Timer16::Timer1);
    constexpr auto OutA = CompareOutputChannel::ChannelB;
    using Pwm = AvrTimer16::FastPwmMode;

    static_assert(pwm1.findFrequency(F_CPU, 900, 1200) == 0);       // frequency range too narrow
    static_assert(pwm1.findFrequency(F_CPU, 1, 10) == 0);           // frequency range too low
    static_assert(pwm1.findFrequency(F_CPU, 100000, 9100200) == 0); // frequency range too high
    static_assert(pwm1.findFrequency(F_CPU, 10000, 40000) != 0);    // accepteable range

    constexpr auto config1 = Pwm::configure(OutA, F_CPU, 10000, 40000);
    static_assert(config1.isValid);
    t1.apply(config1);

    constexpr auto config2 = Pwm::setDutyCycle(0.75f, OutA);
    static_assert(config2.isValid);
    t1.apply(config2);
}

static auto periodicInterrupt(void *) -> void
{
    led.toggle();
}

auto setupLedBlinker() -> void
{
    // freq too low
    assert(timer.enablePeriodicInterrupt(F_CPU, 0.001f, IrqHandler {periodicInterrupt, nullptr}) ==
           false);

    // freq too high
    assert(timer.enablePeriodicInterrupt(F_CPU, 99000000.0f,
                                         IrqHandler {periodicInterrupt, nullptr}) == false);

    // toggle the LED every 1s
    assert(timer.enablePeriodicInterrupt(F_CPU, 1.0f, IrqHandler {periodicInterrupt, nullptr}) ==
           true);
}

auto setupLedBlinkerOptimized() -> void
{
    constexpr auto t5 = Board::makeTimer16(Timer16::Timer5);

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
    t5.apply(cfg3);
}

/* -------------------------------------------------------------------------- */

static constexpr auto useOptimizedVersions = false;

auto appMain() -> void
{
    BoardConfig::setupPinmux();

    if constexpr (useOptimizedVersions) {
        outputSquareWaveOptimized();
        outputPwmOptimized();
        setupLedBlinkerOptimized();
    } else {
        outputSquareWave(squareWave1, 300);
        outputPwm(pwm1, 10000, 40000, 0.75f);
        setupLedBlinker();
    }

    Sys::enableInterrupts();
    while (true)
        ;
}
