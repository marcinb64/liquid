#include "../Sys.h"
#include "../Reg.h"

using namespace liquid;

extern char *__brkval;

static constexpr uint16_t SREG_addr = 0x5F;

static constexpr RegBits<7> GIE(SREG_addr);

unsigned int Sys::getFreeMemory()
{
    char stack;
    return static_cast<unsigned int>(&stack - __brkval);
}

auto Sys::enableInterrupts() -> void
{
    __asm__ __volatile__("sei" ::: "memory");
}

auto Sys::disableInterrupts() -> void
{
    __asm__ __volatile__("cli" ::: "memory");
}

auto Sys::areInterruptsEnabled() -> bool
{
    return GIE;
}

// -----------------------------------------------------------------------------

NoInterruptsGuard::NoInterruptsGuard() : savedState(Sys::areInterruptsEnabled())
{
    Sys::disableInterrupts();
}

NoInterruptsGuard::~NoInterruptsGuard()
{
    if (savedState) Sys::enableInterrupts();
}
