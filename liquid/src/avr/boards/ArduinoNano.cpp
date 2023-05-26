#include "ArduinoNano.h"

#include "../../Reg.h"

namespace liquid
{

auto enableGpioInterrupts() -> void
{
    constexpr uint16_t PCICR = 0x68;
    static RegBits<0> PCIE0(PCICR);
    static RegBits<1> PCIE1(PCICR);
    static RegBits<2> PCIE2(PCICR);
    PCIE0 = 1;
    PCIE1 = 1;
    PCIE2 = 1;
}

}

