#include "AdcImpl.h"
#include "AvrAdc.h"

using namespace liquid;

auto AdcChannel::getRawRange() const -> unsigned int
{
    return 1024;
}

auto AdcChannel::readRaw() const -> unsigned int
{
    return owner->readRaw(channel);
}
