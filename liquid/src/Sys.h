#ifndef LIQUID_SYS_H_
#define LIQUID_SYS_H_

namespace liquid
{

class Sys
{
public:
    static unsigned int getFreeMemory();

    static auto enableInterrupts() -> void;
    static auto disableInterrupts() -> void;
};

} // namespace liquid

#endif
