#include <catch2/catch_test_macros.hpp>

#include "mockAvr.h"
#include <avr/AvrI2c.h>

using namespace liquid;

static constexpr auto testCpuFreq = 16'000'000;

struct TwiRegs {
    static constexpr auto TWBR = 0xB8;
    static constexpr auto TWSR = 0xB9;
    static constexpr auto TWAR = 0xBA;
    static constexpr auto TWDR = 0xBB;
    static constexpr auto TWCR = 0xBC;
    static constexpr auto TWAMR = 0xBD;
};

// Writing 1 to TWINT bit causes the hardware to clear this bit.
// This funciton simulates the hardware clearing TWINT bit.
auto hardwareClearsTwint() -> void
{
    writeMemAt(TwiRegs::TWCR) &= ~(1 << 7);
}

TEST_CASE("Avr I2C - Controller mode")
{
    mockMemReset();
    AvrI2cController dev;

    SECTION("Bitrate calculation")
    {
        static_assert(AvrI2c::getSclFrequency(testCpuFreq, 126, AvrI2c::Prescaler::Div4) ==
                      15625.0f);
        static_assert(AvrI2c::getSclFrequency(testCpuFreq, 12, AvrI2c::Prescaler::Div16) ==
                      40000.0f);

        static_assert(AvrI2c::configureBitrate(testCpuFreq, 15625.0f) ==
                      AvrI2c::BitrateConfig {true, AvrI2c::Prescaler::Div4, 126});
        static_assert(AvrI2c::configureBitrate(testCpuFreq, 40000.0f) ==
                      AvrI2c::BitrateConfig {true, AvrI2c::Prescaler::Div1, 192});
        static_assert(AvrI2c::configureBitrate(testCpuFreq, 20000.0f) ==
                      AvrI2c::BitrateConfig {true, AvrI2c::Prescaler::Div4, 98});
        static_assert(AvrI2c::configureBitrate(testCpuFreq, 5000.0f) ==
                      AvrI2c::BitrateConfig {true, AvrI2c::Prescaler::Div16, 100});
    }

    SECTION("Controller Write")
    {
        uint8_t data[] = {0xAB, 0xCD};
        dev.write(0x22, data, sizeof(data));

        // Start condition
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 2) | (1 << 0)));

        // Start Ack
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x08;
        dev.isr();

        // Address + W
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWDR) == 0x44);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 2) | (1 << 0)));

        // Address Ack
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x18;
        dev.isr();

        // Data byte 0
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWDR) == 0xAB);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 2) | (1 << 0)));

        // Data Ack
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x28;
        dev.isr();

        // Data byte 1
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWDR) == 0xCD);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 2) | (1 << 0)));

        // Data Ack
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x28;
        dev.isr();

        // Send Stop bit
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 4) | (1 << 2) | (1 << 0)));
        dev.isr();

        // Stop condition
        CHECK(dev.getStatus() == AvrI2c::Status::Ok);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 4) | (1 << 2) | (1 << 0)));

        hardwareClearsTwint();
    }

    SECTION("Controller Read")
    {
        uint8_t data[] = {0xff, 0xff};

        // Start condition
        dev.read(0x22, data, sizeof(data));
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 2) | (1 << 0)));

        // Start Ack
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x08;
        dev.isr();

        // Address + R
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWDR) == 0x45);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 2) | (1 << 0)));

        // Address Ack
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x40;
        dev.isr();

        // Continue with data read and return Ack, since there's one more byte left
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 6) | (1 << 2) | (1 << 0)));

        // Data byte 0 received, Ack returned
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x50;
        writeMemAt(TwiRegs::TWDR) = 0xAB;
        dev.isr();

        // Retrieve the data and return Nack, since this is the last byte
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK((int)data[0] == 0xAB);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 2) | (1 << 0)));

        // Data byte 1 received, Nack returned
        hardwareClearsTwint();
        writeMemAt(TwiRegs::TWSR) = 0x58;
        writeMemAt(TwiRegs::TWDR) = 0xCD;
        dev.isr();

        // Send Stop bit
        CHECK(dev.getStatus() == AvrI2c::Status::InProgress);
        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 4) | (1 << 2) | (1 << 0)));
        dev.isr();

        // Retrieve the data
        CHECK(dev.getStatus() == AvrI2c::Status::Ok);
        CHECK((int)data[1] == 0xCD);

        CHECK(memAt(TwiRegs::TWCR) == ((1 << 7) | (1 << 4) | (1 << 2) | (1 << 0)));
        hardwareClearsTwint();
    }
}