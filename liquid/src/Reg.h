#ifndef LIQUID_REG_H_
#define LIQUID_REG_H_

#include <stdint.h>

namespace liquid
{

constexpr static auto HIGH = 1;
constexpr static auto LOW  = 0;

using Sfr8  = volatile uint8_t &;
using Sfr16 = volatile uint16_t &;

constexpr Sfr8 sfr8(uint16_t addr)
{
    return *reinterpret_cast<volatile uint8_t *>(addr);
}

constexpr Sfr16 sfr16(uint16_t addr)
{
    return *reinterpret_cast<volatile uint16_t *>(addr);
}

template <class T> auto setBit(T &reg, int bit, bool high) -> void
{
    if (high)
        reg |= static_cast<uint8_t>(1 << bit);
    else
        reg &= static_cast<uint8_t>(~(1 << bit));
}

template <uint8_t lsb, uint8_t width = 1> struct RegBits {
    constexpr explicit RegBits(uint16_t addr_) : addr(addr_) {}

    void operator=(uint8_t value) const
    {
        if constexpr (width == 1) {
            auto r = reinterpret_cast<volatile uint8_t *>(addr);
            if (value != 0) {
                *r |= static_cast<uint8_t>(1 << lsb);
            } else {
                *r &= static_cast<uint8_t>(~(1 << lsb));
            }
        } else {
            auto r = reinterpret_cast<volatile uint8_t *>(addr); // NOLINT
            *r     = static_cast<uint8_t>((*r & ~mask()) | (value << lsb));
        }
    }

    operator int() const
    {
        auto r = reinterpret_cast<volatile uint8_t *>(addr); // NOLINT
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
