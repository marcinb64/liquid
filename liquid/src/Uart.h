#ifndef LIQUID_UART_H_
#define LIQUID_UART_H_

#include <stdint.h>

namespace liquid
{

class Usart
{
public:
    class Impl;

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

    auto setBaud(long int baud) -> void;
    auto setupUart(long int baud) -> void;
    auto tx(uint8_t data) -> void;
    auto tx(const uint8_t *data, int length) -> void;
    auto rx() -> uint8_t;
    auto readLine(uint8_t *rxbuf, int size) -> int;

    Usart(Impl *impl_) : impl(impl_) {}
    Usart(Usart &&other) = default;
    Usart &operator=(Usart &&other) = default;

private:
    Impl *impl;

    Usart(const Usart &) = delete;
    Usart &operator=(const Usart &) = delete;
};

void installAsStdStreams(Usart &uart);

} // namespace liquid

#endif
