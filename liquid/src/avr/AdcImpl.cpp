#include "AdcImpl.h"

using namespace liquid;
using Class = Adc::Impl;

auto Class::readRaw() const -> unsigned int
{
    
}

/* -------------------------------------------------------------------------- */

auto Adc::readRaw() const -> unsigned int
{
    return impl->readRaw();
}

