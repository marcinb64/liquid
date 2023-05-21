#ifndef LIQUID_GENERIC_AVR_H_
#define LIQUID_GENERIC_AVR_H_

#include "../UartImpl.h"
#include <avr/io.h>

namespace liquid
{

static constexpr auto portPin(int port, int pin) -> int
{
    return port * 8 + pin;
}

constexpr auto gpioToPin(int gpio) -> int
{
    return gpio % 8;
}

constexpr auto gpioToDirReg(int gpio) -> Sfr8
{
    auto i = static_cast<uint8_t>(gpio / 8);
    if (i == 0)
        return DDRB;
    else if (i == 1)
        return DDRC;
    else
        return DDRD;
}

constexpr auto gpioToPort(int gpio) -> Sfr8
{
    auto i = static_cast<uint8_t>(gpio / 8);
    if (i == 0)
        return PORTB;
    else if (i == 1)
        return PORTC;
    else
        return PORTD;
}

inline auto gpioToPinReg(int gpio) -> Sfr8
{
    auto i = static_cast<uint8_t>(gpio / 8);
    if (i == 0)
        return PINB;
    else if (i == 1)
        return PINC;
    else
        return PIND;
}

inline auto gpioToIrqMaskReq(int gpio) -> Sfr8
{
    auto i = static_cast<uint8_t>(gpio / 8);
    if (i == 0)
        return PCMSK0;
    else if (i == 1)
        return PCMSK1;
    else
        return PCMSK2;
}

constexpr auto gpioToPcIntPin(int gpio) -> int
{
    return gpio % 8;
}

/* -------------------------------------------------------------------------- */

struct ArduinoNano {
    static constexpr auto PB = 0;
    static constexpr auto PC = 1;
    static constexpr auto PD = 2;

    struct Gpio {
        // Port B
        static constexpr auto D8 = portPin(PB, 0);
        static constexpr auto D9 = portPin(PB, 1);
        static constexpr auto D10 = portPin(PB, 2);
        static constexpr auto D11 = portPin(PB, 3);
        static constexpr auto D12 = portPin(PB, 4);
        static constexpr auto D13 = portPin(PB, 5);
        // PB6 - XTAL
        // PB7 - XTAL

        // port C
        static constexpr auto A0 = portPin(PC, 0);
        static constexpr auto A1 = portPin(PC, 1);
        static constexpr auto A2 = portPin(PC, 2);
        static constexpr auto A3 = portPin(PC, 3);
        static constexpr auto A4 = portPin(PC, 4);
        static constexpr auto A5 = portPin(PC, 5);

        // port D
        static constexpr auto D0 = portPin(PD, 0);
        static constexpr auto D1 = portPin(PD, 1);
        static constexpr auto D2 = portPin(PD, 2);
        static constexpr auto D3 = portPin(PD, 3);
        static constexpr auto D4 = portPin(PD, 4);
        static constexpr auto D5 = portPin(PD, 5);
        static constexpr auto D6 = portPin(PD, 6);
        static constexpr auto D7 = portPin(PD, 7);

        static constexpr auto BuiltInLed = D13;
    };

    static constexpr uint16_t usartBase[] = {
        0xC0,
    };

    static auto makeUsart(int num) -> Usart
    {
        auto *impl = new Usart::Impl(usartBase[num]);
        return Usart {impl};
    }
};

} // namespace liquid

#endif
