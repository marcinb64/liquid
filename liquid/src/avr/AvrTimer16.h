#ifndef AVRTIMER16_H_
#define AVRTIMER16_H_

#include "../Pwm.h"
#include "../Reg.h"
#include "../SquareWave.h"
#include "../Timer.h"
#include "../util.h"
#include "TimerDefs.h"

#include <assert.h>
#include <math.h>

namespace liquid
{

class AvrTimer16
{
public:
    struct Config {
        uint16_t base;
        uint16_t timskAddr;
        uint16_t tifrAddr;
        int      irqCompA;
    };

private:
    const Config &config;

public:
    constexpr auto TCCRA() const
    {
        struct Bits : SfrBase {
            RegBits<6, 2> COMA {regAddr};
            RegBits<4, 2> COMB {regAddr};
            RegBits<2, 2> COMC {regAddr};
            RegBits<0, 2> WGM10 {regAddr};
        };

        return Bits {config.base};
    }

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

    constexpr auto TCCRB() const
    {
        struct Bits : SfrBase {
            RegBits<7> ICNC {regAddr};
            RegBits<6> ICES {regAddr};
            RegBits<3, 2> WGM32 {regAddr};
            RegBits<0, 3> CS {regAddr};
        };

        return Bits {config.base + 0x01};
    }

    constexpr auto TCCRC() const
    {
        struct Bits : SfrBase {
            RegBits<7> FOCA {regAddr};
            RegBits<6> FOCB {regAddr};
        };

        return Bits {config.base + 0x02};
    }

    inline auto TCNT() const -> Sfr16 & { return sfr16(config.base + 0x04); }
    inline auto ICR() const -> Sfr16 & { return sfr16(config.base + 0x06); }
    inline auto OCRA() const -> Sfr16 & { return sfr16(config.base + 0x08); }
    inline auto OCRB() const -> Sfr16 & { return sfr16(config.base + 0x0a); }
    inline auto OCRC() const -> Sfr16 & { return sfr16(config.base + 0x0c); }

    constexpr auto TIMSK() const
    {
        struct Bits : SfrBase {
            RegBits<5> ICIE {regAddr};
            RegBits<2> OCIEB {regAddr};
            RegBits<1> OCIEA {regAddr};
            RegBits<0> TOIE {regAddr};
        };

        return Bits {config.timskAddr};
    }

    constexpr auto TIFR() const
    {
        struct Bits : SfrBase {
            RegBits<5> ICF {regAddr};
            RegBits<2> OCFB {regAddr};
            RegBits<1> OCFA {regAddr};
            RegBits<0> TOV {regAddr};
        };

        return Bits {config.tifrAddr};
    }

    constexpr static auto calcTop(long cpuFreq, int prescaler, long freq) -> uint16_t
    {
        return static_cast<uint16_t>(cpuFreq / (prescaler * freq) - 1);
    }

    auto writeWgm(int mode) const
    {
        TCCRB().WGM32 = (mode >> 2) & 0x3;
        TCCRA().WGM10 = mode & 0x3;
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

    struct ClockSel {
        uint8_t      value;
        unsigned int prescaler;
    };

    static constexpr ClockSel clockNone = {0, 0};
    static constexpr ClockSel clocksArray[] = {{5, 1024}, {4, 256}, {3, 64}, {2, 8}, {1, 1}};

    struct CTCMode {
        static constexpr auto maxTimerValue = 0xffff;

        static constexpr auto getFreq(unsigned long ioFreq, int prescaler, unsigned long ocr)
            -> float
        {
            return static_cast<float>(ioFreq) /
                   (2.0f * static_cast<float>(prescaler) * (1.0f + static_cast<float>(ocr)));
        }

        static constexpr auto getOcr(unsigned long ioFreq, int prescaler, float freq) -> uint16_t
        {
            return static_cast<uint16_t>(
                static_cast<float>(ioFreq) / 2.0f / static_cast<float>(prescaler) / freq - 1.0f);
        }

        static constexpr auto findClock(unsigned long ioFreq, float freq) -> const ClockSel &
        {
            constexpr int N = sizeof(clocksArray) / sizeof(*clocksArray);
            for (int i = N - 1; i >= 0; --i) {
                auto minFreq = getFreq(ioFreq, clocksArray[i].prescaler, maxTimerValue);
                auto maxFreq = getFreq(ioFreq, clocksArray[i].prescaler, 0);
                if (freq > minFreq && freq < maxFreq) return clocksArray[i];
            }
            return clockNone;
        }

        static constexpr auto configure(unsigned long ioFreq, float freq)
        {
            const auto clock = findClock(ioFreq, freq);
            const auto valid = clock.prescaler != 0;
            const auto ocrValue = valid ? getOcr(ioFreq, clock.prescaler, freq) : 0;

            auto c = [=](const AvrTimer16 &obj) {
                obj.writeWgm(AvrTimer16::WaveformGenerationMode::CtcToOcr);
                obj.TCCRB().CS = clock.value;
                obj.OCRA() = ocrValue;

                return true;
            };

            return ComponentConfig<decltype(c)> {valid, c};
        }

        static constexpr auto configurePeriodicInterrupt(unsigned long ioFreq, float freq, const IrqHandler &handler)
        {
            const auto clock = findClock(ioFreq, freq / 2);
            const auto valid = clock.prescaler != 0;
            const auto ocrValue = valid ? getOcr(ioFreq, clock.prescaler, freq / 2) : 0;

            auto c = [=](const AvrTimer16 &obj) {
                obj.writeWgm(AvrTimer16::WaveformGenerationMode::CtcToOcr);
                obj.TCCRB().CS = clock.value;
                obj.OCRA() = ocrValue;
                obj.TIMSK().OCIEA = 1;
                installIrqHandler(obj.config.irqCompA, handler);

                return true;
            };

            return ComponentConfig<decltype(c)> {valid, c};
        }

        static constexpr auto configureSquareWave(unsigned long ioFreq, float freq)
        {
            const auto clock = findClock(ioFreq, freq);
            const auto valid = clock.prescaler != 0;
            const auto ocrValue = valid ? getOcr(ioFreq, clock.prescaler, freq) : 0;

            auto c = [=](const AvrTimer16 &obj) {
                obj.writeWgm(AvrTimer16::WaveformGenerationMode::CtcToOcr);
                obj.TCCRB().CS = clock.value;
                obj.OCRA() = ocrValue;
                obj.TCCRA().COMA = CompareOuputMode::Toggle;
                
                return true;
            };

            return ComponentConfig<decltype(c)> {valid, c};
        }
        
    };

    struct FastPwmMode {
        using Channel = CompareOutputChannel;

        // Fast PWM 10 bit mode Timer TOP value
        static constexpr auto maxTimerValue = 1023;

        static constexpr auto setDutyCycle(float dutyCycle, Channel channel)
        {
            const auto value = static_cast<unsigned int>(dutyCycle * maxTimerValue);

            auto cfg = [=](const AvrTimer16 &obj) {
                if (channel == Channel::ChannelA)
                    obj.OCRA() = value;
                else if (channel == Channel::ChannelB)
                    obj.OCRB() = value;
                else if (channel == Channel::ChannelC)
                    obj.OCRC() = value;
                else
                    assert(false);
            };

            return ComponentConfig<decltype(cfg)> {true, cfg};
        }

        static constexpr auto findFrequency(unsigned long fCpu, unsigned long min,
                                            unsigned long max) -> unsigned long
        {
            const auto clock = findPrescaler(fCpu, min, max);
            if (clock.prescaler == 0)
                return 0;
            else
                return getPwmFreq(fCpu, clock.prescaler, maxTimerValue);
        }

        static constexpr auto configure(Channel channel, unsigned long fCpu, unsigned long min,
                                        unsigned long max)
        {
            const auto clock = findPrescaler(fCpu, min, max);
            const auto valid = clock.prescaler != 0;

            auto cfg = [=](const AvrTimer16 &obj) {
                if (channel == Channel::ChannelA)
                    obj.TCCRA().COMA = CompareOuputMode::Clear;
                else if (channel == Channel::ChannelB)
                    obj.TCCRA().COMB = CompareOuputMode::Clear;
                else if (channel == Channel::ChannelC)
                    obj.TCCRA().COMC = CompareOuputMode::Clear;
                else
                    assert(false);

                obj.writeWgm(AvrTimer16::WaveformGenerationMode::FastPwm10Bit);
                obj.TCCRB().CS = clock.value;

                return true;
            };

            return ComponentConfig<decltype(cfg)> {valid, cfg};
        }

        static constexpr auto getPwmFreq(unsigned long ioFreq, unsigned int prescaler,
                                         unsigned int top) -> unsigned long
        {
            return ioFreq / (prescaler * (1L + top));
        }

        static constexpr auto findPrescaler(unsigned long ioFreq, unsigned long minFreq,
                                            unsigned long maxFreq) -> const ClockSel &
        {
            for (unsigned int i = 0; i < sizeof(clocksArray) / sizeof(*clocksArray); ++i) {
                auto f = getPwmFreq(ioFreq, clocksArray[i].prescaler, maxTimerValue);
                if (f > minFreq && f < maxFreq) return clocksArray[i];
            }
            return clockNone;
        }
    };

    constexpr AvrTimer16(const Config &config_) : config(config_) {}

    template <class T> auto apply(const ComponentConfig<T> &componentConfig) const
    {
        componentConfig.configFunc(*this);
    }

    friend class PwmImpl;
    friend class TimerImpl16;
    friend class SquareWaveImpl16;
};

/* -------------------------------------------------------------------------- */

class TimerImpl16 : public Timer
{
public:
    using CS = AvrTimer16::ClockSelect;
    using ConfigType = decltype(AvrTimer16::CTCMode::configure(1, 1.0f));

    TimerImpl16(AvrTimer16 timer_) : timer(timer_) {}

    virtual ~TimerImpl16() = default;

    auto enablePeriodicInterrupt(unsigned long fCpu, float freq, const IrqHandler &handler)
        -> bool override
    {
        // CTC frequency calculation from AVR docs is for a square wave output, using toggle mode.
        // Two toggles are needed for 1 full square wave cycle, so f_square_wave = 2 * f_timer.
        const auto config = AvrTimer16::CTCMode::configure(fCpu, freq / 2);
        if (!config) return false;

        installIrqHandler(timer.config.irqCompA, handler);

        timer.apply(config);
        timer.TIMSK().OCIEA = 1;

        return true;
    }

    auto disablePeriodicInterrupt() -> void override { timer.TIMSK().OCIEA = 0; }

    auto stop() -> void override { timer.TCCRB().CS = CS::None; }

private:
    AvrTimer16 timer;
};

/* -------------------------------------------------------------------------- */

class PwmImpl : public Pwm
{
public:
    PwmImpl(AvrTimer16 timer_, CompareOutputChannel channel_) : timer(timer_), channel(channel_) {}

    virtual ~PwmImpl() = default;

    auto setDutyCycle(float dutyCycle) -> void override
    {
        auto config = AvrTimer16::FastPwmMode::setDutyCycle(dutyCycle, channel);
        timer.apply(config);
    }

    constexpr auto findFrequency(unsigned long fCpu, unsigned long min, unsigned long max)
        -> unsigned long
    {
        return AvrTimer16::FastPwmMode::findFrequency(fCpu, min, max);
    }

    auto configure(unsigned long fCpu, unsigned long min, unsigned long max) -> bool override
    {
        auto config = AvrTimer16::FastPwmMode::configure(channel, fCpu, min, max);
        if (!config) return false;
        timer.apply(config);
        return true;
    }

private:
    AvrTimer16                 timer;
    const CompareOutputChannel channel;
};

/* -------------------------------------------------------------------------- */

class SquareWaveImpl16 : public SquareWave
{
public:
    using CS = AvrTimer16::ClockSelect;

    SquareWaveImpl16(AvrTimer16 timer_) : timer(timer_) {}

    virtual ~SquareWaveImpl16() = default;

    template <class T> auto apply(const T &componentConfig) { timer.apply(componentConfig); }

    auto configure(unsigned long fCpu, float freq) -> bool override
    {
        if (!setFrequency(fCpu, freq)) return false;
        timer.TCCRA().COMA = CompareOuputMode::Toggle;
        return true;
    }

    auto setFrequency(unsigned long fCpu, float freq) -> bool override
    {
        const auto config = AvrTimer16::CTCMode::configure(fCpu, freq);
        if (!config) return false;
        timer.apply(config);
        return true;
    }

    constexpr auto tryConfigureFrequency(unsigned long fCpu, float freq)
    {
        return AvrTimer16::CTCMode::configure(fCpu, freq);
    }

    auto enableOutput() -> void override { timer.TCCRA().COMA = CompareOuputMode::Toggle; }

    auto disableOutput() -> void override { timer.TCCRA().COMA = CompareOuputMode::None; }

private:
    AvrTimer16 timer;
};

} // namespace liquid

#endif
