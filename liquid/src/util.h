#ifndef UTIL_H_
#define UTIL_H_

namespace liquid
{

template <class T> struct ComponentConfig {
    bool isValid;
    T    configFunc;

    operator bool() const { return isValid; }
};

template <class T, class E> class Result
{
    bool success {false};

    union Storage {
        T data;
        E error;

        Storage() {}
        Storage(T data_) : data(data_) {}
        Storage(const char, E error_) : error(error_) {}
    } storage;

public:
    Result() : success(false) {}
    Result(const T &data) : success(true), storage(data) {}
    Result(const char, const E &error) : success(false), storage(0, error) {}

    static constexpr auto ok(T data) { return Result<T, E> {data}; }

    static constexpr auto err(const E &error) { return Result<T, E> {0, error}; }

    auto isSuccess() const { return success; }
    auto isError() const { return !success; }
         operator bool() const { return success; }

    auto getValue() const { return storage.data; }
    auto getError() const { return storage.error; }

    template <class T2, class F> auto andThen(F func) -> Result<T2, E>
    {
        if (!success) return Result<T2, E>::err(storage.error);

        return func();
    }
};

template <class E> class Result<void, E>
{
    bool success {false};

    union Storage {
        E error;

        Storage() {}
        Storage(const char, E error_) : error(error_) {}
    } storage;

public:
    Result() : success(false) {}
    Result(const char) : success(true), storage() {}
    Result(const char, const E &error) : success(false), storage(0, error) {}

    static constexpr auto ok() { return Result<void, E> {0}; }

    static constexpr auto err(const E &error) { return Result<void, E> {0, error}; }

    auto isSuccess() const { return success; }
    auto isError() const { return !success; }
         operator bool() const { return success; }

    auto getError() const { return storage.error; }

    template <class T2, class F> auto andThen(F func) -> Result<T2, E>
    {
        if (!success) return Result<T2, E>::err(storage.error);

        return func();
    }
};

template <class T1, class T2, class E>
auto operator&&(const Result<T1, E> &a, const Result<T2, E> &b) -> Result<T2, E>
{
    if (!a) return Result<T2, E>::err(a.getError());
    if (!b) return b;
    return b;
}

} // namespace liquid

#endif
