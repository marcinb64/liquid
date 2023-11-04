#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

namespace liquid
{

class Eeprom {
public:
    static auto readByte(uintptr_t address) -> uint8_t;
    static auto writeByte(uintptr_t address, uint8_t data) -> void;

    static auto readData(uintptr_t address, void *data, int size) -> void;
    static auto writeData(uintptr_t address, const void *data, int size) -> void;

    template<class T>
    static auto readStruct(uintptr_t address, T& obj)
    {
        readData(address, &obj, sizeof(obj));
    }

    template<class T>
    static auto writeStruct(uintptr_t address, const T& obj)
    {
        writeData(address, &obj, sizeof(obj));
    }
};

}

#endif
