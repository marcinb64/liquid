#include "app.h"
#include <Eeprom.h>
#include <Sys.h>
#include <avr/BoardSelector.h>

#include <stdio.h>
#include <string.h>
#include <util/delay.h>

using namespace liquid;

struct BoardConfig {
    static constexpr auto led = Board::Gpio::BuiltInLed;

    static void setupPinmux() { Board::makeGpio(led).asOutput(); }
};

/* -------------------------------------------------------------------------- */

struct Settings {
    uint16_t id {0};
    char     name[8] {0};
};

auto appMain() -> void
{
    BoardConfig::setupPinmux();
    Sys::enableInterrupts();

    _delay_ms(1000);

    Settings s;
    uint8_t x = Eeprom::readByte(0x0005);
    Eeprom::readStruct(0x0006, s);

    printf("x = %d\r\n", x);
    printf("settings = { 0x%04X, '%s' }\n\r", s.id, s.name);

    ++x;
    ++s.id;
    snprintf(s.name, sizeof(s.name), "hi %d", s.id);
    
    _delay_ms(1000);
    Eeprom::writeByte(0x0005, x);
    Eeprom::writeStruct(0x006, s);
    
    while (true) {}
}
