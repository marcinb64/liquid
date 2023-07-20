#include "../../Interrupts.h"
#include "../../SysTimer.h"
#include <avr/interrupt.h>

using namespace liquid;

namespace liquid
{

auto enableGpioInterrupts() -> void
{
    constexpr uint16_t PCICR_ = 0x68;
    static RegBits<0>  PCIE0_(PCICR_);
    static RegBits<1>  PCIE1_(PCICR_);
    static RegBits<2>  PCIE2_(PCICR_);
    PCIE0_ = 1;
    PCIE1_ = 1;
    PCIE2_ = 1;
}

} // namespace liquid

ISR(PCINT0_vect)
{
    callGpioIsr();
}

ISR(PCINT1_vect)
{
    callGpioIsr();
}

ISR(PCINT2_vect)
{
    callGpioIsr();
}

ISR(USART_UDRE_vect)
{
    callUsartIsr();
}
