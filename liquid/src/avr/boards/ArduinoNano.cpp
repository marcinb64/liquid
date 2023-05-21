#include "ArduinoNano.h"

#include "../../Reg.h"

namespace liquid
{

auto enableGpioInterrupts() -> void
{
    setBit(PCICR, PCIE0, true);
    setBit(PCICR, PCIE1, true);
    setBit(PCICR, PCIE2, true);
}

}

