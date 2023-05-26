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

    struct Refs {
        static constexpr auto Aref = 0;
        static constexpr auto Avcc = 1;
        static constexpr auto Reserved1 = 2;
        static constexpr auto Internal1V1 = 3;
    };

    constexpr auto ADCL_addr() const -> uint16_t { return base; }
    inline auto ADCL() const { return sfr8(ADCL_addr()); }

    constexpr auto ADCH_addr() const -> uint16_t { return base + 1; }
    inline auto ADCH() const { return sfr8(ADCH_addr()); }

public:
    constexpr AvrAdc(uint16_t base_) : base(base_)
    {
        ADCSRA().ADEN = 1;
        ADMUX().REFS = Refs::Avcc;
    }

    ~AvrAdc() { ADCSRA().ADEN = 0; }

    auto inline selectChannel(int channel) -> void
    {
        ADCSRB().MUX5 = (channel >> 5) & 0x01;
        ADMUX().MUX40 = channel & 0x1f;
    }

    auto readRaw(int channel) -> unsigned int
    {
        selectChannel(channel);

        ADCSRA().ADSC = 1;
        while (ADCSRA().ADSC == 1)
            ;

        uint8_t lo = ADCL();
        uint8_t hi = ADCH();
        return (hi << 8) | lo;
    }
};

} // namespace liquid

#endif
