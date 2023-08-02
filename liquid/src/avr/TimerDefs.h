#ifndef TIMER_DEFS_H_
#define TIMER_DEFS_H_

namespace liquid
{

enum class Timer16 { Timer1, Timer3, Timer4, Timer5, None };

enum class CompareOutputChannel { None, ChannelA, ChannelB, ChannelC };

struct CompareOuputMode {
    static constexpr auto None = 0;
    static constexpr auto Toggle = 1;
    static constexpr auto Clear = 2;
    static constexpr auto Set = 3;
};

} // namespace liquid

#endif
