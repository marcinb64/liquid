#include "ArduinoNano.h"

#include "../../Reg.h"

namespace liquid
{

static ArduinoNano::Timer0 timer0;

auto ArduinoNano::enableSysTimer() -> void
{
    timer0.setupAsSysTimer(1000 /* Hz */);
}

auto enableGpioInterrupts() -> void
{
    constexpr uint16_t PCICR = 0x68;
    static RegBits<0>  PCIE0(PCICR);
    static RegBits<1>  PCIE1(PCICR);
    static RegBits<2>  PCIE2(PCICR);
    PCIE0 = 1;
    PCIE1 = 1;
    PCIE2 = 1;
}

} // namespace liquid
