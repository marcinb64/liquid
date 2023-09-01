#ifndef PWM_H_
#define PWM_H_

namespace liquid
{

class Pwm
{
public:
    virtual ~Pwm() = default;
    
    virtual auto configure(unsigned long fCpu, unsigned long min, unsigned long max) -> bool = 0;
    virtual auto setDutyCycle(float dutyCycle) -> void = 0;
};

}

#endif
