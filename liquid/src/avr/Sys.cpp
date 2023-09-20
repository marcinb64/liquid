#include "../Sys.h"

using namespace liquid;

extern char *__brkval;


unsigned int Sys::getFreeMemory()
{
    char stack;
    return static_cast<unsigned int>(&stack - __brkval);
}

auto Sys::enableInterrupts() -> void
{
    __asm__ __volatile__ ("sei" ::: "memory");
}

auto Sys::disableInterrupts() -> void
{
    __asm__ __volatile__ ("cli" ::: "memory");
}
