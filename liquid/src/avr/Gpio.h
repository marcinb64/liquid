#ifndef LIQUID_GPIO2_H_
#define LIQUID_GPIO2_H_

#include "../Interrupts.h"
#include "BoardSelector.h"

namespace liquid
{

// board-specific
auto enableGpioInterrupts() -> void;

class Gpio
{
public:
    Gpio(int gpio_) : gpio(gpio_) {}

    enum class Dir { In, Out };
    enum class Pullup { None, PullUp, PullDown };

    auto set(int value) -> void
    {
        auto &reg = gpioToPort(gpio);
        auto  pin = gpioToPin(gpio);
        setBit(reg, pin, value);
    }

    auto setHigh() -> void
    {
        auto &reg = gpioToPort(gpio);
        auto  pin = gpioToPin(gpio);
        setBit(reg, pin, true);
    }

    auto setLow() -> void
    {
        auto &reg = gpioToPort(gpio);
        auto  pin = gpioToPin(gpio);
        setBit(reg, pin, false);
    }

    auto toggle() -> void
    {
        auto &reg = gpioToPort(gpio);
        auto  pin = gpioToPin(gpio);
        reg ^= static_cast<uint8_t>(~(1 << pin));
    }

    auto get() -> int
    {
        auto &reg = gpioToPinReg(gpio);
        auto  pin = gpioToPin(gpio);
        return reg & (1 << pin);
    }

    auto asInput(Pullup pullup) -> void
    {
        auto &dirReg = gpioToDirReg(gpio);
        auto &reg = gpioToPort(gpio);
        auto  pin = gpioToPin(gpio);

        setBit(dirReg, pin, false);
        setBit(reg, pin, pullup == Pullup::PullUp);
    }

    auto asOutput() -> void
    {
        auto &dirReg = gpioToDirReg(gpio);
        auto  pin = gpioToPin(gpio);

        setBit(dirReg, pin, true);
    }

    auto enableInterrupt() -> void
    {
        setBit(gpioToIrqMaskReq(gpio), gpioToPcIntPin(gpio), true);
        enableGpioInterrupts();
    }

    auto disableInterrupt() -> void { setBit(gpioToIrqMaskReq(gpio), gpioToPcIntPin(gpio), false); }

private:
    const int gpio;
};

} // namespace liquid

#endif
