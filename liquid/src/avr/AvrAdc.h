#ifndef AVRADC_H_
#define AVRADC_H_

#include "../Reg.h"

namespace liquid
{

class AvrAdc
{
private:
    const uint16_t base;

    
    constexpr auto ADCSRA_addr() const -> uint16_t { return base + 2; }
    constexpr auto ADCSRA() const
    {
        struct Bits : SfrBase {
            RegBits<7>    ADEN {regAddr};
            RegBits<6>    ADSC {regAddr};
            RegBits<5>    ADATE {regAddr};
            RegBits<4>    ADIF {regAddr};
            RegBits<3>    ADIE {regAddr};
            RegBits<0, 3> ADPS {regAddr};
        };

        return Bits {ADCSRA_addr()};
    }

    constexpr auto ADCSRB_addr() const -> uint16_t { return base + 3; }
    constexpr auto ADCSRB() const
    {
        struct Bits : SfrBase {
            RegBits<6>    ACME {regAddr};
            RegBits<3>    MUX5 {regAddr};
            RegBits<0, 3> ADTS {regAddr};
        };
        return Bits {ADCSRB_addr()};
    }

    constexpr auto ADMUX_addr() const -> uint16_t { return base + 4; }
    constexpr auto ADMUX() const
    {
        struct Bits : SfrBase {
            RegBits<6, 2> REFS {regAddr};
            RegBits<5>    ADLAR {regAddr};
            RegBits<0, 5> MUX40 {regAddr};
        };
        return Bits {ADMUX_addr()};
    }

    constexpr static auto REFS_AREF = 0;
    constexpr static auto REFS_AVCC = 1;
    constexpr static auto REFS_INTERNAL_1V1 = 1;
    constexpr static auto REFS_INTERNAL_2V56 = 1;

public:
    constexpr AvrAdc(uint16_t base_) : base(base_) {}

    auto readRaw(int) const -> unsigned int
    {
        ADCSRA().ADEN = 1;
        while (ADCSRA().ADEN == 1)
            ;
        return 0;
    }
};

} // namespace liquid

#endif
