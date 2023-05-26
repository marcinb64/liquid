#ifndef ADC_H_
#define ADC_H_

namespace liquid
{

class AdcChannel;

class Adc
{
public:
    class Impl;

    Adc(Impl *impl_) : impl(impl_) {}
    Adc(Adc &&other) = default;
    Adc &operator=(Adc &&other) = default;

    auto makeChannel(int channel) -> AdcChannel;
    auto readRaw(int channel) const -> unsigned int;

private:
    Impl *impl;

    Adc(const Adc &other) = delete;
    Adc &operator=(const Adc &other) = delete;
};

/* -------------------------------------------------------------------------- */

class AdcChannel
{
public:
    auto getRawRange() const -> unsigned int;
    auto readRaw() const -> unsigned int;
    // auto readVoltage() const -> float;

private:
    Adc::Impl *owner;
    int        channel;

    AdcChannel(Adc::Impl *owner_, int channel_) : owner(owner_), channel(channel_) {}
    friend class Adc;
};


} // namespace liquid

#endif
