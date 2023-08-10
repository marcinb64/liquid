#ifndef SQUAREWAVE_H_
#define SQUAREWAVE_H_

namespace liquid
{

class SquareWave
{
public:
    virtual ~SquareWave() = default;

    virtual auto configure(unsigned long fCpu, float freq) -> bool = 0;
    virtual auto setFrequency(unsigned long fCpu, float freq) -> bool = 0;
    virtual auto enableOutput() -> void = 0;
    virtual auto disableOutput() -> void = 0;
};

}

#endif
