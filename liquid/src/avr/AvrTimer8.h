#ifndef AVRTIMER8_H_
#define AVRTIMER8_H_

#include "../Reg.h"

namespace liquid
{

class AvrTimer8
{
private:
    const uint16_t base;

    constexpr uint8_t findOcr(long fCpu, long periodUs, int prescaler)
    {
        return static_cast<uint8_t>(fCpu / 1000000 * periodUs / prescaler - 1);
    }

public:
    AvrTimer8(uint16_t baseAddress_) : base(baseAddress_) {}

    
};

} // namespace liquid

#endif
