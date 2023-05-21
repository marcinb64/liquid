#include "../Sys.h"
#include <avr/interrupt.h>

using namespace liquid;

extern char *__brkval;

unsigned int Sys::getFreeMemory()
{
    char stack;
    return &stack - __brkval;
}

auto Sys::enableInterrupts() -> void
{
    sei();
}

auto Sys::disableInterrupts() -> void
{
    cli();
}
