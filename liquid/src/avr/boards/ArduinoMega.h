#ifndef ARDUINO_MEGA_H_
#define ARDUINO_MEGA_H_

#include "../AdcImpl.h"
#include "../AvrInterrupts.h"
#include "../AvrTimer16.h"
#include "../AvrTimer8.h"
#include "../Gpio.h"
#include "../UartImpl.h"

namespace liquid
{

static AvrGpioRegs portA {
    sfr8(0x20), sfr8(0x21), sfr8(0x22),
    sfr8(0), // No PCINT
};

/*
 * PCINT 0 - 7
 * 7 -> OC0A, OC1C
 * 6 -> OC1B
 * 5 -> OC1A
 * 4 -> OC2A
 */
static AvrGpioRegs portB {
    sfr8(0x23), sfr8(0x24), sfr8(0x25),
    sfr8(0x6b), // PCINT 0-7
};

static AvrGpioRegs portC {
    sfr8(0x26), sfr8(0x27), sfr8(0x28),
    sfr8(0), // No PCINT
};

static AvrGpioRegs portD {
    sfr8(0x29), sfr8(0x2a), sfr8(0x2b),
    sfr8(0), // No PCINT
};

/*
 * PCINT8 - bit 0
 * 5 -> OC3C
 * 4 -> OC3B
 * 3 -> OC3A
 */
static AvrGpioRegs portE {
    sfr8(0x2c), sfr8(0x2d), sfr8(0x2e),
    sfr8(0x0C), // PCINT8
};

static AvrGpioRegs portF {
    sfr8(0x2f), sfr8(0x30), sfr8(0x31),
    sfr8(0), // No PCINT
};

static AvrGpioRegs portG {
    sfr8(0x32), sfr8(0x33), sfr8(0x34),
    sfr8(0), // No PCINT
};

/*
 * 6 -> OC2B
 * 5 -> OC4C
 * 4 -> OC4B
 * 3 -> OC4A
 */
static AvrGpioRegs portH {
    sfr8(0x100), sfr8(0x101), sfr8(0x102),
    sfr8(0), // No PCINT
};

static AvrGpioRegs portJ {
    sfr8(0x103), sfr8(0x104), sfr8(0x105),
    sfr8(0x6c), // pins 0-6 -> PCINT 9-15
};

static AvrGpioRegs portK {
    sfr8(0x106), sfr8(0x107), sfr8(0x108),
    sfr8(0x6D), // PCINT 16-23
};

/*
 * 5 -> OC5C
 * 4 -> OC5B
 * 3 -> OC5A
 */
static AvrGpioRegs portL {
    sfr8(0x109), sfr8(0x10a), sfr8(0x10b),
    sfr8(0x6d), // No PCINT
};

/* -------------------------------------------------------------------------- */

class ArduinoMega
{
private:
    static constexpr uint16_t usartBase[] = {
        0xC0, // USART0
        0xC8, // USART1
        0xD0, // USART2
        0x130 // USART3
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
        {
            0x80,
            0x6F,
            0x36,
            Irq::Timer1CompA,
        },
        // Timer 3
        {
            0x90,
            0x71,
            0x38,
            Irq::Timer3CompA,
        },
        // Timer 4
        {
            0xA0,
            0x72,
            0x39,
            Irq::Timer4CompA,
        },
        // Timer 5
        {
            0x120,
            0x73,
            0x3A,
            Irq::Timer5CompA,
        },
    };

public:
    static constexpr auto makeTimer8(Timer8Id num) -> AvrTimer8
    {
        return AvrTimer8(timer8Config[static_cast<int>(num)]);
    }

    static constexpr auto makeTimer16(Timer16 num) -> AvrTimer16
    {
        return AvrTimer16(timer16config[static_cast<int>(num)]);
    }

    struct Gpio {
        static constexpr auto COMA = CompareOutputChannel::ChannelA;
        static constexpr auto COMB = CompareOutputChannel::ChannelB;
        static constexpr auto COMC = CompareOutputChannel::ChannelC;

        static constexpr GpioSpec A0 = {portF, 0};
        static constexpr GpioSpec A1 = {portF, 1};
        static constexpr GpioSpec A2 = {portF, 2};
        static constexpr GpioSpec A3 = {portF, 3};
        static constexpr GpioSpec A4 = {portF, 4};
        static constexpr GpioSpec A5 = {portF, 5};
        static constexpr GpioSpec A6 = {portF, 6};
        static constexpr GpioSpec A7 = {portF, 7};

        static constexpr GpioSpec A8 = {portK, 0, {16, 0}};
        static constexpr GpioSpec A9 = {portK, 1, {17, 1}};
        static constexpr GpioSpec A10 = {portK, 2, {18, 2}};
        static constexpr GpioSpec A11 = {portK, 3, {19, 3}};
        static constexpr GpioSpec A12 = {portK, 4, {20, 4}};
        static constexpr GpioSpec A13 = {portK, 5, {21, 5}};
        static constexpr GpioSpec A14 = {portK, 5, {22, 6}};
        static constexpr GpioSpec A15 = {portK, 5, {23, 7}};

        static constexpr GpioSpec D0 = {portE, 0, {8, 0}};
        static constexpr GpioSpec D1 = {portE, 1};
        static constexpr GpioSpec D2 = {portE, 4, {Timer16::Timer3, COMB}};
        static constexpr GpioSpec D3 = {portE, 5, {Timer16::Timer3, COMC}};
        static constexpr GpioSpec D4 = {portG, 5, {Timer8Id::Timer0, COMB}};
        static constexpr GpioSpec D5 = {portE, 3, {Timer16::Timer3, COMA}};
        static constexpr GpioSpec D6 = {portH, 3, {Timer16::Timer4, COMA}};
        static constexpr GpioSpec D7 = {portH, 4, {Timer16::Timer4, COMB}};

        static constexpr GpioSpec D8 = {portH, 5, {Timer16::Timer4, COMC}};
        static constexpr GpioSpec D9 = {portH, 6, {Timer8Id::Timer2, COMB}};
        static constexpr GpioSpec D10 = {portB, 4, {4, 4}, {Timer8Id::Timer2, COMA}};
        static constexpr GpioSpec D11 = {portB, 5, {5, 5}, {Timer16::Timer1, COMA}};
        static constexpr GpioSpec D12 = {portB, 6, {6, 6}, {Timer16::Timer1, COMB}};
        static constexpr GpioSpec D13 = {portB, 7, {7, 7}, {Timer16::Timer1, COMC}, {Timer8Id::Timer0, COMA}};
        static constexpr GpioSpec D14 = {portJ, 1, {10, 1}};
        static constexpr GpioSpec D15 = {portJ, 0, {9, 0}};

        static constexpr GpioSpec D16 = {portH, 1};
        static constexpr GpioSpec D17 = {portH, 0};
        static constexpr GpioSpec D18 = {portD, 3};
        static constexpr GpioSpec D19 = {portD, 2};
        static constexpr GpioSpec D20 = {portD, 1};
        static constexpr GpioSpec D21 = {portD, 0};
        static constexpr GpioSpec D22 = {portA, 0};
        static constexpr GpioSpec D23 = {portA, 1};

        static constexpr GpioSpec D24 = {portA, 2};
        static constexpr GpioSpec D25 = {portA, 3};
        static constexpr GpioSpec D26 = {portA, 4};
        static constexpr GpioSpec D27 = {portA, 5};
        static constexpr GpioSpec D28 = {portA, 6};
        static constexpr GpioSpec D29 = {portA, 7};
        static constexpr GpioSpec D30 = {portC, 7};
        static constexpr GpioSpec D31 = {portC, 6};

        static constexpr GpioSpec D32 = {portC, 5};
        static constexpr GpioSpec D33 = {portC, 4};
        static constexpr GpioSpec D34 = {portC, 3};
        static constexpr GpioSpec D35 = {portC, 2};
        static constexpr GpioSpec D36 = {portC, 1};
        static constexpr GpioSpec D37 = {portC, 0};
        static constexpr GpioSpec D38 = {portD, 7};
        static constexpr GpioSpec D39 = {portG, 2};

        static constexpr GpioSpec D40 = {portG, 1};
        static constexpr GpioSpec D41 = {portG, 0};
        static constexpr GpioSpec D42 = {portL, 7};
        static constexpr GpioSpec D43 = {portL, 6};
        static constexpr GpioSpec D44 = {portL, 5, {Timer16::Timer5, COMC}};
        static constexpr GpioSpec D45 = {portL, 4, {Timer16::Timer5, COMB}};
        static constexpr GpioSpec D46 = {portL, 3, {Timer16::Timer5, COMA}};
        static constexpr GpioSpec D47 = {portL, 2};

        static constexpr GpioSpec D48 = {portL, 1};
        static constexpr GpioSpec D49 = {portL, 0};
        static constexpr GpioSpec D50 = {portB, 3, {3, 3}};
        static constexpr GpioSpec D51 = {portB, 2, {2, 2}};
        static constexpr GpioSpec D52 = {portB, 1, {1, 1}};
        static constexpr GpioSpec D53 = {portB, 0, {0, 0}};

        static constexpr auto BuiltInLed = D13;
    };

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
