#ifndef TIMER_H_
#define TIMER_H_

#include "Interrupts.h"

namespace liquid
{

class Timer
{
public:
    virtual ~Timer() = default;

    virtual auto enablePeriodicInterrupt(unsigned long fCpu, float freq, const IrqHandler &handler) -> bool = 0;
    virtual auto disablePeriodicInterrupt() -> void = 0;
    virtual auto stop() -> void = 0;
};

}

#endif
