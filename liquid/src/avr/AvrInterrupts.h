#ifndef AVRINTERRUPTS_H_
#define AVRINTERRUPTS_H_

namespace liquid
{

struct Irq {
    static constexpr auto Timer0CompA = 1;
    static constexpr auto Timer1CompA = 2;
    static constexpr auto Timer2CompA = 3;
    static constexpr auto Timer3CompA = 4;
    static constexpr auto Timer4CompA = 5;
    static constexpr auto Timer5CompA = 6;

    static constexpr auto Pcint0 = 5;
    static constexpr auto Pcint1 = 6;
    static constexpr auto Pcint2 = 7;

    static constexpr auto Max = 8;
};

}

#endif
