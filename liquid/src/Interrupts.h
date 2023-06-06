#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

namespace liquid
{

using IrqFunc = auto (*)(void*) -> void;

struct IrqHandler
{
    IrqFunc func;
    void *data;

    auto operator()() const -> void { func(data); }
};

auto addUsartIsr(IrqFunc func, void *data) -> void;
auto callUsartIsr() -> void;

auto installGpioIsr(IrqFunc func, void *data) -> void;
auto callGpioIsr() -> void;

template <class T, void(T::*Method)()>
auto callMethod(void *obj_)
{
    auto *obj = reinterpret_cast<T*>(obj_);
    (obj->*Method)();
}

}

#endif
