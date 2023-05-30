#ifndef PWM_H_
#define PWM_H_

namespace liquid
{

class Pwm
{
public:
    class Impl;

    Pwm(Impl *impl_) : impl(impl_) {}
    Pwm(Pwm &&other) = default;
    Pwm &operator=(Pwm &&other) = default;

    auto set(float dutyCycle) -> void;
    auto requestFrequency(unsigned long min, unsigned long max) -> bool;

private:
    Impl *impl;
    
    Pwm(const Pwm &other) = delete;
    Pwm &operator=(const Pwm &other) = delete;
};

}

#endif