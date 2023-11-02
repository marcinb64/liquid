#ifndef SYSTIMER_H_
#define SYSTIMER_H_

#include "Interrupts.h"
#include "Sys.h"

namespace liquid
{

class SysTimer
{
public:
    template <class TimerClass>
    auto setupWith(TimerClass &timer, unsigned long fCpu, float freq) -> void
    {
        using CTCMode = typename TimerClass::CTCMode;

        auto handler = IrqHandler::callMemberFunc<SysTimer, &SysTimer::isr>(this);
        const auto cfg = CTCMode::configurePeriodicInterrupt(fCpu, freq, handler);
        timer.apply(cfg);
    }

    inline auto getTime() -> unsigned long
    {
        Sys::disableInterrupts();
        auto t = sysTime;
        Sys::enableInterrupts();
        return t;
    }

    inline auto isr() { ++sysTime; }

private:
    unsigned long sysTime = 0;
};

} // namespace liquid

#endif
