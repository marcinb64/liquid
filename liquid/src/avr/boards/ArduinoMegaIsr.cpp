#include "../../Interrupts.h"
#include "../../SysTimer.h"
#include "../AvrInterrupts.h"

#include <avr/interrupt.h>

using namespace liquid;

namespace liquid
{

IrqHandler irqHandlers[5] = {};

auto installIrqHandler(int irq, const IrqHandler &handler) -> void
{
    irqHandlers[irq] = handler;
}

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

ISR(TIMER1_COMPA_vect)
{
    irqHandlers[Irq::Timer1CompA]();
}

ISR(TIMER3_COMPA_vect)
{
    irqHandlers[Irq::Timer3CompA]();
}

ISR(TIMER4_COMPA_vect)
{
    irqHandlers[Irq::Timer4CompA]();
}

ISR(TIMER5_COMPA_vect)
{
    irqHandlers[Irq::Timer5CompA]();
}

ISR(USART1_UDRE_vect)
{
    callUsartIsr();
}
