#include "../../Interrupts.h"
#include "../../SysTimer.h"
#include <avr/interrupt.h>

using namespace liquid;

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

ISR(TIMER0_COMPA_vect)
{
    SysTimer::isr();
}

ISR(USART_UDRE_vect)
{
    callUsartIsr();
}
