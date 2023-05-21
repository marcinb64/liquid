#ifndef LIQUID_UART_IMPL_H_
#define LIQUID_UART_IMPL_H_

#include "../Uart.h"
#include "../Reg.h"
#include "../Interrupts.h"


namespace liquid
{

static constexpr auto calcNormalModeUbrr(long int fOsc, long int baud) -> uint16_t
{
    return static_cast<uint16_t>((fOsc + 8 * baud) / (16 * baud) - 1);
}

static_assert(calcNormalModeUbrr(16000000, 9600) == 103);
static_assert(calcNormalModeUbrr(16000000, 19200) == 51);
static_assert(calcNormalModeUbrr(16000000, 115200) == 8);

class Usart::Impl
{
private:
    const uint16_t base;

    uint16_t ucsrA() const { return base; }
    uint16_t ucsrB() const { return base + 1; }
    uint16_t ucsrC() const { return base + 2; }
    uint16_t ubrr() const { return base + 4; }
    uint16_t udr() const { return base + 6; }

public:
    Impl(uint16_t baseAddr) noexcept : base(baseAddr) {
        addUsartIsr([](void *obj) {
            reinterpret_cast<Usart::Impl*>(obj)->isr();
        }, this);
    }

    auto RXC() { return RegBits<7>(ucsrA()); }
    auto TXC() { return RegBits<6>(ucsrA()); }
    auto UDRE() { return RegBits<5>(ucsrA()); }
    auto FE() { return RegBits<4>(ucsrA()); }
    auto DOR() { return RegBits<3>(ucsrA()); }
    auto UPE() { return RegBits<2>(ucsrA()); }
    auto U2X() { return RegBits<1>(ucsrA()); }
    auto MPCM() { return RegBits<0>(ucsrA()); }

    auto RXCIE() { return RegBits<7>(ucsrB()); }
    auto TXCIE() { return RegBits<6>(ucsrB()); }
    auto UDRIE() { return RegBits<5>(ucsrB()); }
    auto RXEN() { return RegBits<4>(ucsrB()); }
    auto TXEN() { return RegBits<3>(ucsrB()); }
    auto UCSZ2() { return RegBits<2>(ucsrB()); }
    auto RXB8() { return RegBits<1>(ucsrB()); }
    auto TXB8() { return RegBits<0>(ucsrB()); }

    auto UMSEL() { return RegBits<6, 2>(ucsrC()); }
    auto UPM() { return RegBits<4, 2>(ucsrC()); }
    auto USBS() { return RegBits<3>(ucsrC()); }
    auto UCSZ10() { return RegBits<1, 2>(ucsrC()); }
    auto UDORD() { return RegBits<2>(ucsrC()); }
    auto UCPHA() { return RegBits<1>(ucsrC()); }
    auto UCPOL() { return RegBits<0>(ucsrC()); }

    struct UsartMode {
        static constexpr auto ASYNC_USART = 0;
        static constexpr auto SYNC_USART = 1;
        static constexpr auto SPI = 3;
    };

    struct ParityMode {
        static constexpr auto DISABLED = 0;
        static constexpr auto EVEN = 2;
        static constexpr auto ODD = 3;
    };

    struct StopBitSelect {
        static constexpr auto BIT_1 = 0;
        static constexpr auto BIT_2 = 1;
    };

    struct CharSize {
        static constexpr auto SIZE_5_BIT = 0;
        static constexpr auto SIZE_6_BIT = 1;
        static constexpr auto SIZE_7_BIT = 2;
        static constexpr auto SIZE_8_BIT = 3;
        static constexpr auto SIZE_9_BIT = 7;
    };

    auto setBaud(long int baud) { sfr16(ubrr()) = calcNormalModeUbrr(F_CPU, baud); }

    auto setupUart(long int baud)
    {
        setBaud(baud);
        RXEN() = 1;
        TXEN() = 1;
        UCSZ10() = CharSize::SIZE_8_BIT;
    }

    auto tx(uint8_t data)
    {
        while (txLength > 0)
            ;
        while (UDRE() == 0)
            ;
        sfr8(udr()) = data;
    }

    auto tx(const uint8_t *data, int length)
    {
        while (UDRE() == 0)
            ;

        txBuffer = data;
        txLength = length;

        UDRIE() = 1;
    }

    auto rx() -> uint8_t
    {
        while (!RXC())
            ;
        return sfr8(udr());
    }

    auto readLine(uint8_t *rxbuf, int size) -> int
    {
        for (int i = 0; i < size + 1; ++i) {
            uint8_t ch = rx();
            rxbuf[i] = ch;
            if (ch == '\n' || ch == '\r') {
                rxbuf[i] = 0;
                return i;
            }
        }
        return size;
    }

    inline void isr()
    {
        if (txLength > 0) {
            sfr8(udr()) = *txBuffer++;
            --txLength;
        } else {
            UDRIE() = 0;
        }
    }

private:
    const uint8_t *txBuffer;
    int            txLength;
};


} // namespace liquid

#endif
