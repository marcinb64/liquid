#include "../../Interrupts.h"
#include "../../SysTimer.h"
#include "../AvrInterrupts.h"

#include <avr/interrupt.h>

using namespace liquid;

namespace liquid
{

IrqHandler irqHandlers[Irq::Max] = {};

auto installIrqHandler(int irq, const IrqHandler &handler) -> void
{
    irqHandlers[irq] = handler;
}

} // namespace liquid

ISR(PCINT0_vect)
{
    irqHandlers[Irq::Pcint0]();
}

ISR(PCINT1_vect)
{
    irqHandlers[Irq::Pcint0]();
}

ISR(PCINT2_vect)
{
    irqHandlers[Irq::Pcint1]();
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
