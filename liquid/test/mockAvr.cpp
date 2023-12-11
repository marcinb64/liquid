#include "mockAvr.h"
#include <string.h>

uint8_t mock_mem[1024] = {0};

auto mockMemReset() -> void
{
    memset(mock_mem, 0, sizeof(mock_mem));
}

auto memAt(uint16_t addr) -> int
{
    return static_cast<int>(mock_mem[addr]);
}

auto writeMemAt(uint16_t addr) -> uint8_t &
{
    return mock_mem[addr];
}

namespace liquid
{

struct IrqHandler;

auto installIrqHandler(int, const IrqHandler &) -> void
{
}

} // namespace liquid
