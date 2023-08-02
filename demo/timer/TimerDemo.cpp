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
auto outputSquareWave(SquareWave &out, float f) -> void
{
    assert(out.setFrequency(F_CPU, f));
}

/**
 * Output square wave using a hardware timer - optimized variant
 *
 * If all inputs are know at compile time, it offers compile-time frequency
 * calculation and range checking.
 */
auto outputSquareWaveOptimized() -> void
{
    constexpr auto cfg1 = squareWave1.tryConfigureFrequency(F_CPU, 300);
    static_assert(cfg1.isValid);

    constexpr auto cfg2 = squareWave1.tryConfigureFrequency(F_CPU, 99300400);
    static_assert(cfg2.isValid == false);

    constexpr auto cfg3 = squareWave1.tryConfigureFrequency(F_CPU, 0.0001f);
    static_assert(cfg3.isValid == false);

    squareWave1.apply(cfg1);
    squareWave1.enableOutput();
}

/**
 * Output PWM using a hardware timer
 *
 * Calculates the parameters at run time.
 * Can work with any SquareWave implementation, using different hardware timers.
 */
auto outputPwm(Pwm &out, unsigned long f0, unsigned long f1, float v) -> void
{
    assert(out.configure(F_CPU, f0, f1));
    out.set(v);
}

static auto periodicInterrupt(void *) -> void
{
    led.toggle();
}

auto setupLedBlinker() -> void
{
    constexpr auto cfg1 = timer.prepFrequency(F_CPU, 0.001f); // freq too low
    static_assert(cfg1.isValid == false);

    constexpr auto cfg2 = timer.prepFrequency(F_CPU, 99000000.0f); // freq too high
    static_assert(cfg2.isValid == false);

    constexpr auto cfg3 = timer.prepFrequency(F_CPU, 1); // toggle the LED every 1s
    static_assert(cfg3.isValid);
    timer.enablePeriodicInterrupt(cfg3, IrqHandler {periodicInterrupt, nullptr});
}

/* -------------------------------------------------------------------------- */

auto appMain() -> void
{
    BoardConfig::setupPinmux();

    outputSquareWave(squareWave1, 300);
    outputSquareWaveOptimized();

    static_assert(pwm1.findFrequency(F_CPU, 900, 1200) == 0);       // frequency range too narrow
    static_assert(pwm1.findFrequency(F_CPU, 1, 10) == 0);           // frequency range too low
    static_assert(pwm1.findFrequency(F_CPU, 100000, 9100200) == 0); // frequency range too high
    static_assert(pwm1.findFrequency(F_CPU, 10000, 40000) != 0);    // accepteable range
    outputPwm(pwm1, 10000, 40000, 0.75f);

    setupLedBlinker();

    Sys::enableInterrupts();
    while (true)
        ;
}
