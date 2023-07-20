#ifndef AVRINTERRUPTS_H_
#define AVRINTERRUPTS_H_

namespace liquid
{

struct Irq {
    static constexpr auto Timer1CompA = 1;
    static constexpr auto Timer3CompA = 2;
    static constexpr auto Timer4CompA = 3;
    static constexpr auto Timer5CompA = 4;
};

}

#endif
