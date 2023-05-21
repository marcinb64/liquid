#ifndef ADC_H_
#define ADC_H_

namespace liquid
{

class Adc
{
public:
    class Impl;

    Adc(Impl * impl);
    Adc(Adc &&other) = default;
    Adc &operator=(Adc &&other) = default;

    auto readRaw() const -> unsigned int;

private:
    Impl *impl;

    Adc(const Adc &other) = delete;
    Adc &operator=(const Adc &other) = delete;
};

}

#endif
