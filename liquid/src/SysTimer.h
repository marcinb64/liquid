#ifndef SYSTIMER_H_
#define SYSTIMER_H_

#include "Sys.h"
#include "Interrupts.h"

namespace liquid
{

class SysTimer
{
public:
    template<class TimerClass>
    auto setupWith(TimerClass &timer, unsigned long fCpu, float freq) -> void
    {
        IrqHandler handler { callMethod<SysTimer, &SysTimer::isr>, this };
        timer.enablePeriodicInterrupt(fCpu, freq, handler);
    }

    inline auto getTime() -> unsigned long
    {
        Sys::disableInterrupts();
        auto t = sysTime;
        Sys::enableInterrupts();
        return t;
    }

    inline auto isr()
    {
        ++sysTime;
    }

private:
    unsigned long sysTime;
};

}

#endif
