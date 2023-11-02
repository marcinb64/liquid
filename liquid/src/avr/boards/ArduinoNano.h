#ifndef ARDUINO_NANO_AVR_H_
#define ARDUINO_NANO_AVR_H_

#include "../AdcImpl.h"
#include "../AvrInterrupts.h"
#include "../AvrTimer16.h"
#include "../AvrTimer8.h"
#include "../UartImpl.h"

#include "../Gpio.h"

namespace liquid
{

// PCINT 0-7 on pins 0-7
static AvrGpioRegs portB {
    sfr8(0x23),
    sfr8(0x24),
    sfr8(0x25),
    sfr8(0x6b),
};

// PCINT 8-14 on pins 0-6
static AvrGpioRegs portC {
    sfr8(0x26),
    sfr8(0x27),
    sfr8(0x28),
    sfr8(0x6c),
};

// PCINT 16-23 on pins 0-7
static AvrGpioRegs portD {
    sfr8(0x29),
    sfr8(0x2a),
    sfr8(0x2b),
    sfr8(0x6d),
};

/* -------------------------------------------------------------------------- */

class ArduinoNano
{
private:
    static constexpr uint16_t usartBase[] = {
        0xC0,
    };

public:
    struct Gpio {
        static constexpr auto COMA = CompareOutputChannel::ChannelA;
        static constexpr auto COMB = CompareOutputChannel::ChannelB;

        static constexpr GpioSpec D8 = {portB, 0, {0, 0}};
        static constexpr GpioSpec D9 = {portB, 1, {1, 1}, {Timer16::Timer1, COMA}};
        static constexpr GpioSpec D10 = {portB, 2, {2, 2}, {Timer16::Timer1, COMB}};
        static constexpr GpioSpec D11 = {portB, 3, {3, 3}};
        static constexpr GpioSpec D12 = {portB, 4, {4, 4}};
        static constexpr GpioSpec D13 = {portB, 5, {5, 5}};
        // PB6 - XTAL
        // PB7 - XTAL

        static constexpr GpioSpec A0 = {portC, 0, {8, 0}};
        static constexpr GpioSpec A1 = {portC, 1, {9, 1}};
        static constexpr GpioSpec A2 = {portC, 2, {10, 2}};
        static constexpr GpioSpec A3 = {portC, 3, {11, 3}};
        static constexpr GpioSpec A4 = {portC, 4, {12, 4}};
        static constexpr GpioSpec A5 = {portC, 5, {13, 5}};
        // PC6 - RESET

        static constexpr GpioSpec D0 = {portD, 0, {16, 0}};
        static constexpr GpioSpec D1 = {portD, 1, {17, 1}};
        static constexpr GpioSpec D2 = {portD, 2, {18, 2}};
        static constexpr GpioSpec D3 = {portD, 3, {19, 3}};
        static constexpr GpioSpec D4 = {portD, 4, {20, 4}};
        static constexpr GpioSpec D5 = {portD, 5, {21, 5}};
        static constexpr GpioSpec D6 = {portD, 6, {22, 6}};
        static constexpr GpioSpec D7 = {portD, 7, {23, 7}};

        static constexpr auto BuiltInLed = D13;
    };

    static constexpr AvrTimer8::Config timer8Config[] = {
        // Timer 0
        {
            0x44,
            0x6e,
            0x35,
            Irq::Timer0CompA,
        },
    };

    static constexpr AvrTimer16::Config timer16config[] = {
        // Timer 1
        {0x80, 0x6F, 0x36, Irq::Timer1CompA},
    };

    static constexpr auto makeTimer8(Timer8Id num) -> AvrTimer8
    {
        return AvrTimer8(timer8Config[static_cast<int>(num)]);
    }

    static constexpr auto makeTimer16(Timer16 num) -> AvrTimer16
    {
        return AvrTimer16(timer16config[static_cast<int>(num)]);
    }

    static auto makeGpio(const GpioSpec &spec) { return liquid::Gpio(spec, spec.pin); }

    static auto makePwm(const GpioSpec &spec) -> PwmImpl
    {
        assert(spec.pwm16.timer != Timer16::None &&
               spec.pwm16.channel != CompareOutputChannel::None);
        return PwmImpl {makeTimer16(spec.pwm16.timer), spec.pwm16.channel};
    }

    static auto makeSquareWave(const GpioSpec &spec) -> SquareWaveImpl16
    {
        // Square wave uses CTC mode, only Compare Output Channel A can be used
        assert(spec.pwm16.timer != Timer16::None &&
               spec.pwm16.channel == CompareOutputChannel::ChannelA);
        return SquareWaveImpl16(makeTimer16(spec.pwm16.timer));
    }

    static auto makeAdc() -> Adc { return Adc {new Adc::Impl(0x78)}; }

    static auto makeUsart(int num) -> Usart { return Usart {new Usart::Impl(usartBase[num])}; }
};

} // namespace liquid

#endif
