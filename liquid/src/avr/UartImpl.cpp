#include "UartImpl.h"

#include <stdint.h>
#include <stdio.h>

using namespace liquid;

static FILE console = {};

static int uart_putchar(char c, FILE *stream)
{
    static_cast<liquid::Usart*>(stream->udata)->tx(static_cast<uint8_t>(c));
    return 0;
}

static int uart_getchar(FILE *stream)
{
    return static_cast<liquid::Usart*>(stream->udata)->rx();
}

namespace liquid
{

void installAsStdStreams(Usart &uart)
{
    fdev_setup_stream(&console, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    fdev_set_udata(&console, &uart);
    stdout = &console;
    stderr = &console;
    stdin = &console;
}

auto Usart::setBaud(unsigned long fCpu, unsigned long baud) -> void
{
    impl->setBaud(fCpu, baud);
}

auto Usart::setupUart(unsigned long fCpu, unsigned long baud) -> void
{
    impl->setupUart(fCpu, baud);
}

auto Usart::tx(uint8_t data) -> void
{
    impl->tx(data);
}

auto Usart::tx(const void *data, int length) -> void
{
    impl->tx(reinterpret_cast<const uint8_t*>(data), length);
}

auto Usart::rx() -> uint8_t
{
    return impl->rx();
}

auto Usart::isRxReady() const -> bool
{
    return impl->isRxReady();
}

auto Usart::readLine(uint8_t *rxbuf, int size) -> int
{
    return impl->readLine(rxbuf, size);
}

} // namespace liquid
