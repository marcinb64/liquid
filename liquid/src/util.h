#ifndef UTIL_H_
#define UTIL_H_

namespace liquid
{

template <class T> struct ComponentConfig {
    bool isValid;
    T    configFunc;

    operator bool() const { return isValid; }
};

}

#endif
