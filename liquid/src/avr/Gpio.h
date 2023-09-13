#ifndef LIQUID_GPIO2_H_
#define LIQUID_GPIO2_H_

#include "../Interrupts.h"
#include "AvrInterrupts.h"

namespace liquid
{

static constexpr auto PCICR_ADDR = 0x68;

struct AvrGpioRegs {
    Sfr8 pin;
    Sfr8 dir;
    Sfr8 port;
    Sfr8 pcmsk;
};

struct GpioSpec {
    struct Pwm16 {
        Timer16              timer = Timer16::None;
        CompareOutputChannel channel = CompareOutputChannel::None;
    };

    struct Pwm8 {
        Timer8Id             timer = Timer8Id::None;
        CompareOutputChannel channel = CompareOutputChannel::None;
    };

    struct Pcint {
        int     pcint = -1;
        uint8_t pcmskMask = 0;

        constexpr Pcint() {}

        constexpr Pcint(int pcint_, int bit)
            : pcint(pcint_), pcmskMask(static_cast<uint8_t>(1 << bit))
        {
            assert(bit >= 0 && bit <= 7);
        }

        constexpr auto getIrq() const -> int
        {
            return pcint != -1 ? Irq::Pcint0 + (pcint / 8) : -1;
        }
    };

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_) : regs(regs_), pin(pin_) {}

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_, const Pcint &pcint_)
        : regs(regs_), pin(pin_), pcint(pcint_)
    {
    }

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_, const Pcint &pcint_, Pwm16 pwm16_)
        : regs(regs_), pin(pin_), pcint(pcint_), pwm16(pwm16_)
    {
    }

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_, const Pcint &pcint_, Pwm8 pwm8_)
        : regs(regs_), pin(pin_), pcint(pcint_), pwm8(pwm8_)
    {
    }

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_, const Pcint &pcint_, Pwm16 pwm16_, Pwm8 pwm8_)
        : regs(regs_), pin(pin_), pcint(pcint_), pwm16(pwm16_), pwm8(pwm8_)
    {
    }

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_, Pwm16 pwm16_)
        : regs(regs_), pin(pin_), pwm16(pwm16_)
    {
    }

    constexpr GpioSpec(AvrGpioRegs &regs_, int pin_, Pwm8 pwm8_)
        : regs(regs_), pin(pin_), pwm8(pwm8_)
    {
    }

    AvrGpioRegs &regs;
    int          pin;
    Pcint        pcint;
    Pwm16        pwm16 = {Timer16::None, CompareOutputChannel::None};
    Pwm8         pwm8 = {Timer8Id::None, CompareOutputChannel::None};

    constexpr bool operator==(const GpioSpec &other) const
    {
        return (&regs == &other.regs) && (pin == other.pin);
    }

    constexpr auto getIrq() const -> int { return pcint.getIrq(); }
};

class Gpio
{
public:
    Gpio(const GpioSpec &spec_, int gpio_)
        : spec(spec_), portReg(spec.regs.port), pinMask(static_cast<uint8_t>(1 << (gpio_ % 8)))
    {
    }

    enum class Dir { In, Out };
    enum class Pullup { None, PullUp };

    inline auto set(int value) -> void { writeByMask(portReg, pinMask, value); }

    inline auto setHigh() -> void { set(1); }

    inline auto setLow() -> void { set(0); }

    inline auto toggle() -> void { portReg ^= static_cast<uint8_t>(pinMask); }

    inline auto get() -> int { return spec.regs.pin & pinMask; }

    auto asInput(Pullup pullup) -> void
    {
        writeByMask(spec.regs.dir, pinMask, 0);
        writeByMask(portReg, pinMask, pullup == Pullup::PullUp);
    }

    auto asOutput() -> void { writeByMask(spec.regs.dir, pinMask, 1); }

    inline auto enableInterrupt() -> void
    {
        writeByMask(spec.regs.pcmsk, spec.pcint.pcmskMask, 1);
        enableGpioInterrupts(pciintToBank(spec.pcint.pcint));
    }

    inline auto disableInterrupt() -> void
    {
        writeByMask(spec.regs.pcmsk, spec.pcint.pcmskMask, 0);
    }

private:
    const GpioSpec     &spec;
    const volatile Sfr8 portReg;
    const uint8_t       pinMask;

    static inline auto pciintToBank(int pcint) -> int { return pcint / 8; }

    static inline auto enableGpioInterrupts(int bank) -> void
    {
        writeBit(sfr8(PCICR_ADDR), bank, true);
    }
};

} // namespace liquid

#endif
