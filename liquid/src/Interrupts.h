#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

namespace liquid
{

using IrqFunc = auto (*)(void*) -> void;

constexpr static auto InvalidIrq = -1;

template <class T, void(T::*Method)()>
auto callMemberFunc(void *obj_)
{
    auto *obj = reinterpret_cast<T*>(obj_);
    (obj->*Method)();
}

struct IrqHandler
{
    IrqFunc func;
    void *data;

    auto operator()() const -> void { func(data); }

    template <class T, void(T::*Method)()>
    static IrqHandler callMemberFunc(void *obj_)
    {
        return IrqHandler { liquid::callMemberFunc<T, Method>, obj_ };
    }
};

auto addUsartIsr(IrqFunc func, void *data) -> void;
auto callUsartIsr() -> void;

constexpr auto isValidIrq(int irq) -> bool
{
    return irq != InvalidIrq;
}

auto installIrqHandler(int irq, const IrqHandler &handler) -> void;

}

#endif
