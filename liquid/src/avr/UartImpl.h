#ifndef LIQUID_UART_IMPL_H_
#define LIQUID_UART_IMPL_H_

#include "../Uart.h"
#include "../Reg.h"
#include "../Interrupts.h"


namespace liquid
{

static constexpr auto calcNormalModeUbrr(unsigned long fOsc, unsigned long baud) -> uint16_t
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

    auto RXC() const { return RegBits<7>(ucsrA()); }
    auto TXC() const { return RegBits<6>(ucsrA()); }
    auto UDRE() const { return RegBits<5>(ucsrA()); }
    auto FE() const { return RegBits<4>(ucsrA()); }
    auto DOR() const { return RegBits<3>(ucsrA()); }
    auto UPE() const { return RegBits<2>(ucsrA()); }
    auto U2X() const { return RegBits<1>(ucsrA()); }
    auto MPCM() const { return RegBits<0>(ucsrA()); }

    auto RXCIE() const { return RegBits<7>(ucsrB()); }
    auto TXCIE() const { return RegBits<6>(ucsrB()); }
    auto UDRIE() const { return RegBits<5>(ucsrB()); }
    auto RXEN() const { return RegBits<4>(ucsrB()); }
    auto TXEN() const { return RegBits<3>(ucsrB()); }
    auto UCSZ2() const { return RegBits<2>(ucsrB()); }
    auto RXB8() const { return RegBits<1>(ucsrB()); }
    auto TXB8() const { return RegBits<0>(ucsrB()); }

    auto UMSEL() const { return RegBits<6, 2>(ucsrC()); }
    auto UPM() const { return RegBits<4, 2>(ucsrC()); }
    auto USBS() const { return RegBits<3>(ucsrC()); }
    auto UCSZ10() const { return RegBits<1, 2>(ucsrC()); }
    auto UDORD() const { return RegBits<2>(ucsrC()); }
    auto UCPHA() const { return RegBits<1>(ucsrC()); }
    auto UCPOL() const { return RegBits<0>(ucsrC()); }

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

    auto setBaud(unsigned long fCpu, unsigned long baud) { sfr16(ubrr()) = calcNormalModeUbrr(fCpu, baud); }

    auto setupUart(unsigned long fCpu, unsigned long baud)
    {
        setBaud(fCpu, baud);
        UCSZ10() = CharSize::SIZE_8_BIT;
        RXEN() = 1;
        TXEN() = 1;
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
        while (txLength > 0)
            ;
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

    auto isRxReady() const -> bool
    {
        return RXC();
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
    const uint8_t *txBuffer = nullptr;
    int            txLength = 0;
};


} // namespace liquid

#endif
