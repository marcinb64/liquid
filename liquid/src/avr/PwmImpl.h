#ifndef PWMIMPL_H_
#define PWMIMPL_H_

#include "../Pwm.h"
#include "AvrTimer16.h"

namespace liquid
{

class Pwm::Impl
{
private:
    static constexpr auto maxTimerValue = 1023;

    static constexpr auto getPwmFreq(unsigned long ioFreq, unsigned int prescaler, unsigned int top)
        -> unsigned long
    {
        return ioFreq / (prescaler * (1L + top));
    }

    static constexpr unsigned int prescalers[] = {1024, 256, 64, 8, 1};
    static constexpr unsigned int clockSel[] = {
        AvrTimer16::ClockSelect::ClkIoDiv1024, AvrTimer16::ClockSelect::ClkIoDiv256,
        AvrTimer16::ClockSelect::ClkIoDiv64, AvrTimer16::ClockSelect::ClkIoDiv8,
        AvrTimer16::ClockSelect::ClkIo};

    static constexpr auto findPrescaler(unsigned long ioFreq, unsigned long minFreq) -> int
    {
        for (unsigned int i = 0; i < sizeof(prescalers) / sizeof(*prescalers); ++i) {
            auto pwmFreq = getPwmFreq(ioFreq, prescalers[i], maxTimerValue);
            if (pwmFreq > minFreq) return clockSel[i];
        }
        return 0;
    }

public:
    Impl(AvrTimer16::Channel channel_) : channel(channel_)
    {
        constexpr auto clock = findPrescaler(F_CPU, 500);
        static_assert(clock != AvrTimer16::ClockSelect::None);
        timer.setupFastPwm(channel_, clock);
    }

    auto set(float dutyCycle) -> void
    {
        auto value = static_cast<unsigned int>(dutyCycle * maxTimerValue);
        timer.setPwmDutyCycle(channel, value);
    }

private:
    AvrTimer16          timer;
    AvrTimer16::Channel channel;
};

inline auto Pwm::set(float dutyCycle) -> void
{
    return impl->set(dutyCycle);
}

} // namespace liquid

#endif
