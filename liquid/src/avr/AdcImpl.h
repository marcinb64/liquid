#ifndef ADCIMPL_H_
#define ADCIMPL_H_

#include "../Adc.h"
#include "../Reg.h"

namespace liquid
{

class Adc::Impl
{
public:
    Impl(uint16_t baseAddr) : base(baseAddr) {}

    auto readRaw() const -> unsigned int;
    
private:
    uint16_t base;
    
};
}

#endif
