#ifndef AVRTIMER16_H_
#define AVRTIMER16_H_

#include "../Reg.h"

#include <assert.h>
#include <math.h>

namespace liquid
{

class AvrTimer16
{
public:
    static constexpr uint16_t base = 0x80;

    constexpr auto TCCR1A() const
    {
        struct Bits : SfrBase {
            RegBits<6, 2> COM1A {regAddr};
            RegBits<4, 2> COM1B {regAddr};
            RegBits<0, 2> WGM10 {regAddr};
        };

        return Bits {base};
    }

    enum class Channel { ChannelA, ChannelB };

    struct CompareOuputMode {
        static constexpr auto None = 0;
        static constexpr auto Toggle = 1;
        static constexpr auto Clear = 2;
        static constexpr auto Set = 3;
    };

    struct WaveformGenerationMode {
        static constexpr auto Normal = 0;
        static constexpr auto PhaseCorrectPwm8Bit = 1;
        static constexpr auto PhaseCorrectPwm9Bit = 2;
        static constexpr auto PhaseCorrectPwm10Bit = 3;
        static constexpr auto CtcToOcr = 4;
        static constexpr auto FastPwm8Bit = 5;
        static constexpr auto FastPwm9Bit = 6;
        static constexpr auto FastPwm10Bit = 7;
        static constexpr auto PhaseFreqCorrectPwmToIcr = 8;
        static constexpr auto PhaseFreqCorrectPwmToOcr = 9;
        static constexpr auto PhaseCorrectPwmToIcr = 10;
        static constexpr auto PhaseCorrectPwmToOcr = 11;
        static constexpr auto CtcToIcr = 12;
        // reserved
        static constexpr auto FastPwmToIcr1 = 14;
        static constexpr auto FastPwmToOcr = 15;
    };

    constexpr auto TCCR1B() const
    {
        struct Bits : SfrBase {
            RegBits<3, 2> WGM32 {regAddr};
            RegBits<0, 3> CS {regAddr};
        };

        return Bits {base + 0x01};
    }

    constexpr auto TCCR1C() const
    {
        struct Bits : SfrBase {
            RegBits<7> FOC1A {regAddr};
            RegBits<6> FOC1B {regAddr};
        };

        return Bits {base + 0x02};
    }

    inline auto TCNT1() const -> Sfr16 & { return sfr16(base + 0x04); }
    inline auto ICR() const -> Sfr16 & { return sfr16(base + 0x06); }
    inline auto OCRA() const -> Sfr16 & { return sfr16(base + 0x08); }
    inline auto OCRB() const -> Sfr16 & { return sfr16(base + 0x0a); }

    constexpr auto TIMSK() const
    {
        struct Bits : SfrBase {
            RegBits<5> ICIE {regAddr};
            RegBits<2> OCIE1B {regAddr};
            RegBits<1> OCIE1A {regAddr};
            RegBits<0> TOIE {regAddr};
        };

        return Bits {0x6f};
    }

    constexpr auto TIFR() const
    {
        struct Bits : SfrBase {
            RegBits<5> ICF {regAddr};
            RegBits<2> OCF1B {regAddr};
            RegBits<1> OCF1A {regAddr};
            RegBits<0> TOV {regAddr};
        };

        return Bits {0x36};
    }

    struct ClockSelect {
        static constexpr auto None = 0;
        static constexpr auto ClkIo = 1;
        static constexpr auto ClkIoDiv8 = 2;
        static constexpr auto ClkIoDiv64 = 3;
        static constexpr auto ClkIoDiv256 = 4;
        static constexpr auto ClkIoDiv1024 = 5;
        static constexpr auto ExtFaling = 6;
        static constexpr auto ExtRising = 7;
    };

    constexpr static auto calcTop(long cpuFreq, int prescaler, long freq) -> uint16_t
    {
        return static_cast<uint16_t>(cpuFreq / (prescaler * freq) - 1);
    }

    auto writeWgm(int mode)
    {
        TCCR1B().WGM32 = (mode >> 2) & 0x3;
        TCCR1A().WGM10 = mode & 0x3;
    }

public:
    auto setupFastPwm(Channel channel, uint8_t clockSelect)
    {
        writeWgm(WaveformGenerationMode::FastPwm10Bit);

        if (channel == Channel::ChannelA)
            TCCR1A().COM1A = CompareOuputMode::Clear;
        else if (channel == Channel::ChannelB)
            TCCR1A().COM1B = CompareOuputMode::Clear;
        else
            assert(false);

        TCCR1B().CS = clockSelect;
    }

    auto setPwmDutyCycle(Channel ch, unsigned int value)
    {
        if (ch == Channel::ChannelA)
            OCRA() = value;
        else if (ch == Channel::ChannelB)
            OCRB() = value;
        else
            assert(false);
    }
};

} // namespace liquid

#endif
