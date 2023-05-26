#ifndef ADCIMPL_H_
#define ADCIMPL_H_

#include "../Adc.h"
#include "AvrAdc.h"

namespace liquid
{

class Adc::Impl
{
public:
    constexpr Impl(uint16_t base) : adc(base) {}
    auto readRaw(int channel) const -> unsigned int { return adc.readRaw(channel); }

private:
    AvrAdc adc;
};

/* -------------------------------------------------------------------------- */

inline auto Adc::makeChannel(int channel) -> AdcChannel
{
    return AdcChannel(impl, channel);
}

inline auto Adc::readRaw(int channel) const -> unsigned int
{
    return impl->readRaw(channel);
}


} // namespace liquid

#endif
