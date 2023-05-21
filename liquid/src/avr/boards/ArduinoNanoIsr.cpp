#include "../../Interrupts.h"
#include <avr/interrupt.h>

using namespace liquid;

#if 0
ISR(USART_UDRE_vect)
{
    callUsartIsr();
}
#endif

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
