#include "../Eeprom.h"
#include "../Reg.h"
#include "../Sys.h"

using namespace liquid;

namespace liquid {

static constexpr uint16_t EEAR_addr = 0x41;
static constexpr uint16_t EEDR_addr = 0x40;
static constexpr uint16_t EECR_addr = 0x3F;

static constexpr RegBits<4, 2> EEPM {EECR_addr};        // Programming Mode
static constexpr RegBits<3>    EERIE {EECR_addr};       // Ready Interrupt Enable
static constexpr RegBits<2>    EEMPE {EECR_addr};       // Master Program Enable
static constexpr RegBits<1>    EEPE {EECR_addr};        // Program Enable
static constexpr RegBits<0>    EERE {EECR_addr};        // Read Enable

enum EepromProgrammingMode {
    EraseAndWrite = 0,
    EraseOnly = 1,
    WriteOnly = 2,
};

static inline auto waitForWriteComplete() -> void
{
    while (EEPE != 0);
}

static inline auto doRead(uintptr_t address) -> uint8_t
{
    sfr16(EEAR_addr) = address;
    EERE = 1;
    return sfr8(EEDR_addr);
}

static inline auto doWrite(uintptr_t address, uint8_t data) -> void
{
    sfr16(EEAR_addr) = address;
    sfr8(EEDR_addr) = data;
    EEMPE = 1;
    EEPE = 1;
}

}

auto Eeprom::readByte(uintptr_t address) -> uint8_t
{
    NoInterruptsGuard guard;

    waitForWriteComplete();
    return doRead(address);
}

auto Eeprom::readData(uintptr_t address, void *data_, int size) -> void
{
    NoInterruptsGuard guard;
    uint8_t *data = reinterpret_cast<uint8_t*>(data_);

    waitForWriteComplete();

    for (int i = 0; i < size; ++i) {
        data[i] = doRead(address++);
    }
}

auto Eeprom::writeByte(uintptr_t address, uint8_t data) -> void
{
    NoInterruptsGuard guard;

    waitForWriteComplete();
    doWrite(address, data);
}

auto Eeprom::writeData(uintptr_t address, const void *data_, int size) -> void
{
    NoInterruptsGuard guard;
    const uint8_t *data = reinterpret_cast<const uint8_t*>(data_);

    waitForWriteComplete();
    
    for (int i = 0; i < size; ++i) {
        doWrite(address++, data[i]);
        waitForWriteComplete();
    }
}

