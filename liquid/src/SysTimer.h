#ifndef SYSTIMER_H_
#define SYSTIMER_H_

#include "Sys.h"

namespace liquid
{

class SysTimer
{
public:
    static inline auto getTime() -> unsigned long
    {
        Sys::disableInterrupts();
        auto t = sysTime;
        Sys::enableInterrupts();
        return t;
    }

    static inline auto isr()
    {
        ++sysTime;
    }

private:
    static unsigned long sysTime;
};

}

#endif
