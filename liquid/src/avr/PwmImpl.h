#ifndef PWMIMPL_H_
#define PWMIMPL_H_

#include "../Pwm.h"
#include "AvrTimer16.h"

namespace liquid
{

class Pwm::Impl
{
private:
    using CS = AvrTimer16::ClockSelect;

    static constexpr auto maxTimerValue = 1023;

    static constexpr auto getPwmFreq(unsigned long ioFreq, unsigned int prescaler, unsigned int top)
        -> unsigned long
    {
        return ioFreq / (prescaler * (1L + top));
    }

    static constexpr unsigned int prescalers[] = {1024, 256, 64, 8, 1};
    static constexpr uint8_t      clockSel[] = {CS::ClkIoDiv1024, CS::ClkIoDiv256, CS::ClkIoDiv64,
                                                CS::ClkIoDiv8, CS::ClkIo};

    static constexpr auto findPrescaler(unsigned long ioFreq, unsigned long minFreq,
                                        unsigned long maxFreq) -> uint8_t
    {
        for (unsigned int i = 0; i < sizeof(prescalers) / sizeof(*prescalers); ++i) {
            auto f = getPwmFreq(ioFreq, prescalers[i], maxTimerValue);
            if (f > minFreq && f < maxFreq) return clockSel[i];
        }
        return CS::None;
    }

public:
    Impl(AvrTimer16::Channel channel_) : channel(channel_)
    {
        constexpr auto clock = findPrescaler(F_CPU, 500, F_CPU);
        static_assert(clock != CS::None);
        timer.setupFastPwm(channel_, clock);
    }

    auto set(float dutyCycle) -> void
    {
        auto value = static_cast<unsigned int>(dutyCycle * maxTimerValue);
        timer.setPwmDutyCycle(channel, value);
    }

    auto requestFrequency(unsigned long min, unsigned long max) -> bool
    {
        auto clock = findPrescaler(F_CPU, min, max);
        if (clock != CS::None) {
            timer.setupFastPwm(channel, clock);
            return true;
        } else {
            return false;
        }
    }

private:
    AvrTimer16          timer;
    AvrTimer16::Channel channel;
};

inline auto Pwm::set(float dutyCycle) -> void
{
    return impl->set(dutyCycle);
}

inline auto Pwm::requestFrequency(unsigned long min, unsigned long max) -> bool
{
    return impl->requestFrequency(min, max);
}

} // namespace liquid

#endif
