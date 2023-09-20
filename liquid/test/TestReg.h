#ifndef LIQUID_REG_H_
#define LIQUID_REG_H_

#include <stdint.h>

extern uint8_t mock_mem[1024];

namespace liquid
{

constexpr static auto HIGH = 1;
constexpr static auto LOW  = 0;

using Sfr8  = volatile uint8_t &;
using Sfr16 = volatile uint16_t &;

Sfr8 sfr8(uint16_t addr)
{
    return *(mock_mem + addr);
}

Sfr16 sfr16(uint16_t addr)
{
    return *reinterpret_cast<volatile uint16_t *>(mock_mem + addr);
}

inline auto writeBit(Sfr8 &reg, int bit, bool high) -> void
{
    if (high)
        reg |= static_cast<uint8_t>(1 << bit);
    else
        reg &= static_cast<uint8_t>(~(1 << bit));
}

inline auto writeByMask(Sfr8 &reg, uint8_t mask, bool high) -> void
{
    if (high)
        reg |= mask;
    else
        reg &= static_cast<uint8_t>(~mask);
}

struct SfrBase
{
    const uint16_t regAddr;

    constexpr SfrBase(uint16_t addr_) : regAddr(addr_) {}
};

template <uint8_t lsb, uint8_t width = 1> struct RegBits {
    constexpr explicit RegBits(uint16_t addr_) : addr(addr_) {}

    void operator=(uint8_t value) const
    {
        if constexpr (width == 1) {
            auto r = reinterpret_cast<volatile uint8_t *>(mock_mem + addr);
            if (value != 0) {
                *r |= static_cast<uint8_t>(1 << lsb);
            } else {
                *r &= static_cast<uint8_t>(~(1 << lsb));
            }
        } else {
            auto r = reinterpret_cast<volatile uint8_t *>(mock_mem + addr);
            *r     = static_cast<uint8_t>((*r & ~mask()) | (value << lsb));
        }
    }

    operator int() const
    {
        auto r = reinterpret_cast<volatile uint8_t *>(mock_mem + addr);
        return *r & mask();
    }

    [[nodiscard]] static constexpr uint8_t mask()
    {
        uint8_t m = 0;
        for (int i = 0; i < width; i++)
            m |= static_cast<uint8_t>(1 << (lsb + i));
        return m;
    }

    const uint16_t addr;
};

} // namespace liquid

#endif
