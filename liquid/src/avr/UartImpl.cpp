#include "UartImpl.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

using namespace liquid;

static FILE _stdout = {};

static int uart_putchar(char c, FILE *stream)
{
    static_cast<liquid::Usart*>(stream->udata)->tx(static_cast<uint8_t>(c));
    return 0;
}

namespace liquid
{

void installAsStdStreams(Usart &uart)
{
    fdev_setup_stream(&_stdout, uart_putchar, nullptr, _FDEV_SETUP_WRITE);
    fdev_set_udata(&_stdout, &uart);
    stdout = &_stdout;
}

auto Usart::setBaud(long int baud) -> void
{
    impl->setBaud(baud);
}

auto Usart::setupUart(long int baud) -> void
{
    impl->setupUart(baud);
}

auto Usart::tx(uint8_t data) -> void
{
    impl->tx(data);
}

auto Usart::tx(const uint8_t *data, int length) -> void
{
    impl->tx(data, length);
}

auto Usart::rx() -> uint8_t
{
    return impl->rx();
}

auto Usart::readLine(uint8_t *rxbuf, int size) -> int
{
    return impl->readLine(rxbuf, size);
}

} // namespace liquid
