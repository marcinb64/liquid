#ifndef LIQUID_GPIO2_H_
#define LIQUID_GPIO2_H_

#include "../Interrupts.h"

namespace liquid
{

// board-specific
auto enableGpioInterrupts() -> void;

struct AvrGpioRegs {
    Sfr8 pin;
    Sfr8 dir;
    Sfr8 port;
    Sfr8 irqMask;
};

class Gpio
{
public:
    Gpio(const AvrGpioRegs &regs_, int gpio_)
        : regs(regs_), pinMask(static_cast<uint8_t>(1 << (gpio_ % 8)))
    {
    }

    enum class Dir { In, Out };
    enum class Pullup { None, PullUp };

    inline auto set(int value) -> void { writeByMask(regs.port, pinMask, value); }

    inline auto setHigh() -> void { set(1); }

    inline auto setLow() -> void { set(0); }

    inline auto toggle() -> void { regs.port ^= static_cast<uint8_t>(~pinMask); }

    inline auto get() -> int { return regs.pin & pinMask; }

    auto asInput(Pullup pullup) -> void
    {
        writeByMask(regs.dir, pinMask, 0);
        writeByMask(regs.port, pinMask, pullup == Pullup::PullUp);
    }

    auto asOutput() -> void { writeByMask(regs.dir, pinMask, 1); }

    inline auto enableInterrupt() -> void
    {
        writeByMask(regs.irqMask, pinMask, 1);
        enableGpioInterrupts();
    }

    inline auto disableInterrupt() -> void { writeByMask(regs.irqMask, pinMask, 0); }

private:
    const AvrGpioRegs &regs;
    const uint8_t      pinMask;
};

} // namespace liquid

#endif
