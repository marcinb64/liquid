#include "../Interrupts.h"

using namespace liquid;

static IrqHandler usartIrqHandler {nullptr, nullptr};

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

} // namespace liquid
