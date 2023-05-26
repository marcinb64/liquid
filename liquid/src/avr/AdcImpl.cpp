#include "AdcImpl.h"
#include "AvrAdc.h"

using namespace liquid;

auto AdcChannel::getRawRange() const -> unsigned int
{
    return 1024;
}

auto AdcChannel::readRaw() -> unsigned int
{
    return owner->readRaw(channel);
}
