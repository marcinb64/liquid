#ifndef TIMER_DEFS_H_
#define TIMER_DEFS_H_

namespace liquid
{

enum class Timer8Id { Timer0, Timer2, None };

enum class Timer16 { Timer1, Timer3, Timer4, Timer5, None };

enum class CompareOutputChannel { None, ChannelA, ChannelB, ChannelC };

struct CompareOuputMode {
    static constexpr auto None = 0;
    static constexpr auto Toggle = 1;
    static constexpr auto Clear = 2;           // Non-PWM mode
    static constexpr auto Set = 3;             // Non-PWM mode
    static constexpr auto NonInverting = 2;    // Fast PWM mode
    static constexpr auto Inverting = 3;       // Fast PWM mode
    static constexpr auto ClearCountingUp = 2; // Phase correct PWM mode
    static constexpr auto SetCountingUp = 3;   // Phase correct PWM mode
};

} // namespace liquid

#endif
