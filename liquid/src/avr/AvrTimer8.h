#ifndef AVRTIMER8_H_
#define AVRTIMER8_H_

#include "../Reg.h"

namespace liquid
{

template <uint16_t base, uint16_t timskAddr, uint16_t tifrAddr> class AvrTimer8
{
private:
    struct CompareOuputMode {
        static constexpr auto None = 0;
        static constexpr auto Toggle = 1;
        static constexpr auto Clear = 2;           // Non-PWM mode
        static constexpr auto Set = 3;             // Non-PWM mode
        static constexpr auto NonInverting = 2;    // Fast PWM mode
        static constexpr auto Inverting = 3;       // Fast PWM mode
        static constexpr auto ClearCountingUp = 2; // Phase correct PWM mode
        static constexpr auto SetCountingUp = 3;   // Phase correct PWM mode
    };

    struct WaveformGenerationMode {
        static constexpr auto Normal = 0;
        static constexpr auto PhaseCorrectPwm = 1;
        static constexpr auto Ctc = 2;
        static constexpr auto FastPwm = 3;
        // reserved = 4
        static constexpr auto PhaseCorrectPwmToOcrA = 5;
        // reserved = 6
        static constexpr auto FastPwmToOcrA = 7;
    };

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

    constexpr auto TCCRA() const
    {
        struct Bits : SfrBase {
            RegBits<6, 2> COMA {regAddr};
            RegBits<4, 2> COMB {regAddr};
            RegBits<0, 2> WGM10 {regAddr};
        };

        return Bits {base};
    }

    constexpr auto TCCRB() const
    {
        struct Bits : SfrBase {
            RegBits<3>    WGM2 {regAddr};
            RegBits<0, 3> CS {regAddr};
        };

        return Bits {base + 0x01};
    }

    constexpr auto TIMSK() const
    {
        struct Bits : SfrBase {
            RegBits<2> OCIEB {regAddr};
            RegBits<1> OCIEA {regAddr};
            RegBits<0> TOIE {regAddr};
        };

        return Bits {timskAddr};
    }

    constexpr auto TIFR() const
    {
        struct Bits : SfrBase {
            RegBits<2> OCF1B {regAddr};
            RegBits<1> OCF1A {regAddr};
            RegBits<0> TOV {regAddr};
        };

        return Bits {tifrAddr};
    }

    inline auto TCNT() const -> Sfr8 & { return sfr8(base + 0x02); }
    inline auto OCRA() const -> Sfr8 & { return sfr8(base + 0x03); }
    inline auto OCRB() const -> Sfr8 & { return sfr8(base + 0x04); }

    auto writeWgm(int mode)
    {
        TCCRB().WGM2 = (mode >> 2) & 0x1;
        TCCRA().WGM10 = mode & 0x3;
    }

public:
    static constexpr auto getCtcFreq(unsigned long ioFreq, int prescaler, unsigned long ocr)
        -> unsigned long
    {
        return ioFreq / (2 * prescaler * (1 + ocr));
    }

    static constexpr auto getCtcOcr(unsigned long ioFreq, int prescaler, unsigned long freq) -> uint8_t
    {
        return static_cast<uint8_t>(ioFreq / 2 / prescaler / freq - 1);
    }

    auto setupAsSysTimer(unsigned long freq)
    {
        auto ocr = getCtcOcr(F_CPU, 64, freq / 2);
        writeWgm(WaveformGenerationMode::Ctc);
        OCRA() = ocr;
        TCCRB().CS = ClockSelect::ClkIoDiv64;
        TIMSK().OCIEA = 1;
    }
};

} // namespace liquid

#endif
