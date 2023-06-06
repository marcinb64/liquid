#include "../Interrupts.h"

using namespace liquid;

static IrqHandler usartIrqHandler {nullptr, nullptr};
static IrqHandler gpioIrqHandler {nullptr, nullptr};

namespace liquid
{

auto addUsartIsr(IrqFunc func, void *data) -> void
{
    usartIrqHandler = {func, data};
}

auto callUsartIsr() -> void
{
    usartIrqHandler.func(usartIrqHandler.data);
}

auto installGpioIsr(IrqFunc func, void *data) -> void
{
    gpioIrqHandler = {func, data};
}

auto callGpioIsr() -> void
{
    gpioIrqHandler.func(gpioIrqHandler.data);
}


} // namespace liquid
