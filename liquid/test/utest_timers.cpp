#include <catch2/catch_test_macros.hpp>

#include "mockAvr.h"
#include <avr/AvrTimer16.h>
#include <avr/AvrTimer8.h>

using namespace liquid;

struct Timer16Regs {
    static constexpr auto TIMSK1 = 0x6F;

    static constexpr auto TCCR1A = 0x80;
    static constexpr auto TCCR1B = 0x81;
    static constexpr auto TCCR1C = 0x82;
    static constexpr auto TCNT1H = 0x85;
    static constexpr auto TCNT1L = 0x84;
    static constexpr auto OCR1AH = 0x89;
    static constexpr auto OCR1AL = 0x88;
    static constexpr auto OCR1BH = 0x8B;
    static constexpr auto OCR1BL = 0x8A;
    static constexpr auto OCR1CH = 0x8D;
    static constexpr auto OCR1CL = 0x8C;
};

struct Timer8Regs {
    static constexpr auto TIMSK0 = 0x6E;

    static constexpr auto TCCR0A = 0x44;
    static constexpr auto TCCR0B = 0x45;
    static constexpr auto TCNT0 = 0x46;
    static constexpr auto OCR0A = 0x47;
    static constexpr auto OCR0B = 0x48;
};

static AvrTimer8::Config t0cfg {
    0x44,
    0x6E,
    0x35,
    100,
};

static AvrTimer16::Config t1cfg {
    0x80,
    0x6F,
    0x36,
    101,
};

// -----------------------------------------------------------------------------

TEST_CASE("AvrTimer8-CTC")
{
    mockMemReset();
    AvrTimer8 t0(t0cfg);

    SECTION("badFreq")
    {
        CHECK(AvrTimer8::CTCMode::configure(F_CPU, 0.001f).isValid == false);
        CHECK(AvrTimer8::CTCMode::configure(F_CPU, 99300400).isValid == false);
    }

    SECTION("generic")
    {
        constexpr auto cfg = AvrTimer8::CTCMode::configure(F_CPU, 4000);
        static_assert(cfg.isValid);
        t0.apply(cfg);

        CHECK(memAt(Timer8Regs::TCCR0A) == 0x02);
        CHECK(memAt(Timer8Regs::TCCR0B) == 0x02);
        CHECK(memAt(Timer8Regs::OCR0A) == 0xf9);
        CHECK(memAt(Timer8Regs::OCR0B) == 0x00);
        CHECK(memAt(Timer8Regs::TIMSK0) == 0x00);
    }
    SECTION("periodicInterrupt")
    {
        auto           handler = [](void *) {};
        constexpr auto cfg =
            AvrTimer8::CTCMode::configurePeriodicInterrupt(F_CPU, 8000, {handler, nullptr});
        static_assert(cfg.isValid);
        t0.apply(cfg);

        CHECK(memAt(Timer8Regs::TCCR0A) == 0x02);
        CHECK(memAt(Timer8Regs::TCCR0B) == 0x02);
        CHECK(memAt(Timer8Regs::OCR0A) == 0xf9);
        CHECK(memAt(Timer8Regs::OCR0B) == 0x00);
        CHECK(memAt(Timer8Regs::TIMSK0) == 0x02);
    }

    SECTION("squareWave")
    {
        constexpr auto cfg = AvrTimer8::CTCMode::configureSquareWave(F_CPU, 4000);
        static_assert(cfg.isValid);
        t0.apply(cfg);

        CHECK(memAt(Timer8Regs::TCCR0A) == 0x42);
        CHECK(memAt(Timer8Regs::TCCR0B) == 0x02);
        CHECK(memAt(Timer8Regs::OCR0A) == 0xf9);
        CHECK(memAt(Timer8Regs::OCR0B) == 0x00);
        CHECK(memAt(Timer8Regs::TIMSK0) == 0x00);
    }
}

TEST_CASE("AvrTimer8-FastPWM")
{
    AvrTimer8 t0(t0cfg);
    mockMemReset();

    SECTION("findFrequency")
    {
        using Pwm = AvrTimer8::FastPwmMode;
        static_assert(Pwm::findFrequency(F_CPU, 900, 950) == 0); // frequency range too narrow
        static_assert(Pwm::findFrequency(F_CPU, 1, 10) == 0);     // frequency range too low
        static_assert(Pwm::findFrequency(F_CPU, 100000, 9100200) == 0); // frequency range too high
        static_assert(Pwm::findFrequency(F_CPU, 4000, 8000) != 0);    // accepteable range
    }

    SECTION("configure")
    {
        constexpr auto cfg1 =
            AvrTimer8::FastPwmMode::configure(CompareOutputChannel::ChannelB, F_CPU, 4000, 8000);
        static_assert(cfg1.isValid);
        t0.apply(cfg1);

        constexpr auto cfg2 =
            AvrTimer8::FastPwmMode::setDutyCycle(0.75f, CompareOutputChannel::ChannelB);
        static_assert(cfg2.isValid);
        t0.apply(cfg2);

        CHECK(memAt(Timer8Regs::TCCR0A) == 0x23);
        CHECK(memAt(Timer8Regs::TCCR0B) == 0x02);
        CHECK(memAt(Timer8Regs::OCR0A) == 0x00);
        CHECK(memAt(Timer8Regs::OCR0B) == 0xbf);
        CHECK(memAt(Timer8Regs::TIMSK0) == 0x00);
    }
}


// -----------------------------------------------------------------------------

TEST_CASE("AvrTimer16-CTC")
{
    AvrTimer16 t1(t1cfg);
    mockMemReset();

    SECTION("badFreq")
    {
        CHECK(AvrTimer16::CTCMode::configure(F_CPU, 0.001f).isValid == false);
        CHECK(AvrTimer16::CTCMode::configure(F_CPU, 99300400).isValid == false);
    }

    SECTION("generic")
    {
        auto cfg = AvrTimer16::CTCMode::configure(F_CPU, 4000);
        REQUIRE(cfg.isValid);
        t1.apply(cfg);

        CHECK(memAt(Timer16Regs::TCCR1A) == 0);
        CHECK(memAt(Timer16Regs::TCCR1B) == (0x08 | 0x01));
        CHECK(memAt(Timer16Regs::TCCR1C) == 0);
        CHECK(memAt(Timer16Regs::OCR1AH) == 0x07);
        CHECK(memAt(Timer16Regs::OCR1AL) == 0xcf);
        CHECK(memAt(Timer16Regs::TIMSK1) == 0x00);
    }

    SECTION("periodicInterrupt")
    {
        auto handler = [](void *) {};
        auto cfg = AvrTimer16::CTCMode::configurePeriodicInterrupt(F_CPU, 8000, {handler, nullptr});
        REQUIRE(cfg.isValid);
        t1.apply(cfg);

        CHECK(memAt(Timer16Regs::TCCR1A) == 0);
        CHECK(memAt(Timer16Regs::TCCR1B) == (0x08 | 0x01));
        CHECK(memAt(Timer16Regs::TCCR1C) == 0);
        CHECK(memAt(Timer16Regs::OCR1AH) == 0x07);
        CHECK(memAt(Timer16Regs::OCR1AL) == 0xcf);
        CHECK(memAt(Timer16Regs::TIMSK1) == 0x02);
    }

    SECTION("squareWave")
    {
        auto cfg = AvrTimer16::CTCMode::configureSquareWave(F_CPU, 4000);
        REQUIRE(cfg.isValid);
        t1.apply(cfg);

        CHECK(memAt(Timer16Regs::TCCR1A) == 0x40);
        CHECK(memAt(Timer16Regs::TCCR1B) == (0x08 | 0x01));
        CHECK(memAt(Timer16Regs::TCCR1C) == 0);
        CHECK(memAt(Timer16Regs::OCR1AH) == 0x07);
        CHECK(memAt(Timer16Regs::OCR1AL) == 0xcf);
        CHECK(memAt(Timer16Regs::TIMSK1) == 0x00);
    }
}

TEST_CASE("AvrTimer16-FastPWM")
{
    AvrTimer16 t1(t1cfg);
    mockMemReset();

    SECTION("findFrequency")
    {
        using Pwm = AvrTimer16::FastPwmMode;
        static_assert(Pwm::findFrequency(F_CPU, 900, 1200) == 0); // frequency range too narrow
        static_assert(Pwm::findFrequency(F_CPU, 1, 10) == 0);     // frequency range too low
        static_assert(Pwm::findFrequency(F_CPU, 100000, 9100200) == 0); // frequency range too high
        static_assert(Pwm::findFrequency(F_CPU, 10000, 40000) != 0);    // accepteable range
    }

    SECTION("configure")
    {
        constexpr auto cfg1 =
            AvrTimer16::FastPwmMode::configure(CompareOutputChannel::ChannelB, F_CPU, 10000, 40000);
        static_assert(cfg1.isValid);
        t1.apply(cfg1);

        constexpr auto cfg2 =
            AvrTimer16::FastPwmMode::setDutyCycle(0.75f, CompareOutputChannel::ChannelB);
        static_assert(cfg2.isValid);
        t1.apply(cfg2);

        CHECK(memAt(Timer16Regs::TCCR1A) == 0x23);
        CHECK(memAt(Timer16Regs::TCCR1B) == (0x08 | 0x01));
        CHECK(memAt(Timer16Regs::TCCR1C) == 0);
        CHECK(memAt(Timer16Regs::OCR1BH) == 0x02);
        CHECK(memAt(Timer16Regs::OCR1BL) == 0xFF);
        CHECK(memAt(Timer16Regs::TIMSK1) == 0x00);
    }
}
