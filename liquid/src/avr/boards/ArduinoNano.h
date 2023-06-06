#ifndef LIQUID_GENERIC_AVR_H_
#define LIQUID_GENERIC_AVR_H_

#include "../AdcImpl.h"
#include "../PwmImpl.h"
#include "../UartImpl.h"
#include "../AvrTimer8.h"

#include "../Gpio.h"

namespace liquid
{

static AvrGpioRegs portB {
    sfr8(0x23),
    sfr8(0x24),
    sfr8(0x25),
    sfr8(0x6b),
};

static AvrGpioRegs portC {
    sfr8(0x26),
    sfr8(0x27),
    sfr8(0x28),
    sfr8(0x6c),
};

static AvrGpioRegs portD {
    sfr8(0x29),
    sfr8(0x2a),
    sfr8(0x2b),
    sfr8(0x6d),
};

static constexpr auto portPin(int port, int pin) -> int
{
    return port * 8 + pin;
}

/* -------------------------------------------------------------------------- */

class ArduinoNano
{
private:
    struct GpioSpec {
        AvrGpioRegs &regs;
        int          pin;

        constexpr bool operator ==(const GpioSpec &other) const {
            return (&regs == &other.regs) && (pin == other.pin);
        }
    };

    static constexpr uint16_t usartBase[] = {
        0xC0,
    };


public:
    using Timer0 = AvrTimer8<0x44, 0x6e, 0x36>;
    
    struct Gpio {
        static constexpr GpioSpec D8 = {portB, 0};
        static constexpr GpioSpec D9 = {portB, 1};  // OC1A
        static constexpr GpioSpec D10 = {portB, 2}; // OC1B
        static constexpr GpioSpec D11 = {portB, 3};
        static constexpr GpioSpec D12 = {portB, 4};
        static constexpr GpioSpec D13 = {portB, 5};
        // PB6 - XTAL
        // PB7 - XTAL

        static constexpr GpioSpec A0 = {portC, 0};
        static constexpr GpioSpec A1 = {portC, 1};
        static constexpr GpioSpec A2 = {portC, 2};
        static constexpr GpioSpec A3 = {portC, 3};
        static constexpr GpioSpec A4 = {portC, 4};
        static constexpr GpioSpec A5 = {portC, 5};

        static constexpr GpioSpec D0 = {portD, 0};
        static constexpr GpioSpec D1 = {portD, 1};
        static constexpr GpioSpec D2 = {portD, 2};
        static constexpr GpioSpec D3 = {portD, 3};
        static constexpr GpioSpec D4 = {portD, 4};
        static constexpr GpioSpec D5 = {portD, 5};
        static constexpr GpioSpec D6 = {portD, 6};
        static constexpr GpioSpec D7 = {portD, 7};

        static constexpr auto BuiltInLed = D13;
    };

    static auto enableSysTimer() -> void;

    static auto makeGpio(const GpioSpec &spec) { return liquid::Gpio(spec.regs, spec.pin); }

    static auto makePwmD9()
    {
        return Pwm {new Pwm::Impl {AvrTimer16::Channel::ChannelA}};
    }

    static auto makePwmD10()
    {
        return Pwm {new Pwm::Impl {AvrTimer16::Channel::ChannelB}};
    }

    static auto makeAdc() -> Adc { return Adc {new Adc::Impl(0x78)}; }

    static auto makeUsart(int num) -> Usart { return Usart {new Usart::Impl(usartBase[num])}; }
};

} // namespace liquid

#endif
