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

ISR(TIMER0_COMPA_vect)
{
    irqHandlers[Irq::Timer0CompA]();
}

ISR(USART_UDRE_vect)
{
    callUsartIsr();
}
