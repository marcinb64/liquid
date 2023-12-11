#include "app.h"
#include <avr/AvrI2c.h>
#include <avr/BoardSelector.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <util/delay.h>

using namespace liquid;

struct BoardConfig {
    static constexpr auto led = Board::Gpio::BuiltInLed;

    static void setupPinmux() { Board::makeGpio(led).asOutput(); }
};

static auto led = Board::makeGpio(BoardConfig::led);

// -----------------------------------------------------------------------------

auto describeStatus(AvrI2cController::Status result) -> const char *
{
    switch (result) {
    case AvrI2cController::Status::Ok: return "OK";
    case AvrI2cController::Status::InProgress: return "InProgress";
    case AvrI2cController::Status::Nack: return "Nack";
    case AvrI2cController::Status::ArbitrationLost: return "ArbitrationLost";
    case AvrI2cController::Status::BusError: return "BusError";
    case AvrI2cController::Status::Unknown: return "Unknown";
    }
    return "?";
}

struct AHT20 {
    struct Measurement {
        float temperature;
        float humidity;
    };

    using ErrType = AvrI2c::Status;
    using StatusResult = Result<uint8_t, ErrType>;
    using VoidResult = Result<void, ErrType>;
    using MeasureResult = Result<Measurement, ErrType>;

    static constexpr inline auto address = 0x38;

    struct Cmd {
        static inline uint8_t readStatus[] = {0x71};
        static inline uint8_t initialize[] = {0xBE, 0x08, 0x00};
        static inline uint8_t measure[] = {0xAC, 0x33, 0x00};
    };

    AvrI2cController &bus;

    AHT20(AvrI2cController &bus_) : bus(bus_) {}

    auto readStatus() -> StatusResult
    {
        return bus
            .blockingWrite(address, Cmd::readStatus, sizeof(Cmd::readStatus)) //
            .andThen<uint8_t>([&]() {
                return bus.blockingReadByte(address);
            });
    }

    auto initialize() -> VoidResult
    {
        auto r = bus.blockingWrite(address, Cmd::initialize, sizeof(Cmd::initialize));
        if (r) {
            return VoidResult::ok();
        } else {
            return VoidResult::err(r.getError());
        }
    }

    auto measure() -> MeasureResult
    {
        uint8_t data[7] = {0};

        auto r1 = bus.blockingWrite(address, Cmd::measure, sizeof(Cmd::measure));
        if (!r1) {
            return MeasureResult::err(r1.getError());
        }

        auto r2 = waitForMeasurement();
        if (!r2) {
            return MeasureResult::err(r2.getError());
        }

        auto r3 = bus.blockingRead(address, data, sizeof(data));
        if (!r3) {
            return MeasureResult::err(r3.getError());
        }

        auto humidity = convertHumidity(data);
        auto temp = convertTemperature(data);
        return MeasureResult::ok({temp, humidity});
    }

    auto waitForMeasurement() -> StatusResult
    {
        while (true) {
            auto s = readStatus();
            // return on failure or when the busy bit is clear
            if (!s || (s.getValue() & 0x80) == 0) {
                return s;
            }

            _delay_ms(10);
        }
    }

    constexpr auto convertHumidity(const uint8_t *data) -> float
    {
        uint32_t byte1 = (data[1] >> 4) & 0x0F;
        uint32_t byte2 = ((data[1] << 4) & 0xF0) | ((data[2] >> 4) & 0x0F);
        uint32_t byte3 = ((data[2] << 4) & 0xF0) | ((data[3] >> 4) & 0x0F);
        uint32_t Sh = ((byte1 << 16) & 0x00FF0000) | //
                      ((byte2 << 8) & 0x0000FF00) |  //
                      ((byte3 << 0) & 0x000000FF);

        return ((float)Sh / 1048576.0f) * 100.0f;
    }

    auto convertTemperature(const uint8_t *data) -> float
    {
        uint32_t byte1 = data[3] & 0x0F;
        uint32_t byte2 = data[4];
        uint32_t byte3 = data[5];
        uint32_t St = ((byte1 << 16) & 0x00FF0000) | //
                      ((byte2 << 8) & 0x0000FF00) |  //
                      ((byte3 << 0) & 0x000000FF);

        return ((float)St / 1048576.0f) * 200.0f - 50.0f;
    }
};

static auto printFixedPoint(float f) -> void
{
    auto i = static_cast<int>(f);
    auto d = static_cast<int>((f - i) * 100);

    printf("%d.%02d", i, d);
}

struct AHT20App {
    static constexpr auto measurementIntervalMsec = 2000;

    AvrI2cController &bus;
    AHT20             sensor;

    AHT20App(AvrI2cController &bus_) : bus(bus_), sensor(bus) {}

    auto start() -> void
    {
        printf("# Initialize sensor...");
        auto res = sensor.initialize();
        printf("%s\r\n", res ? "OK" : describeStatus(res.getError()));
    }

    auto scan() -> void
    {
        printf("# Scan I2C bus... ");
        bus.scan([](uint8_t address, bool present) {
            if (present) {
                printf("found 0x%02x\r\n", address);
            } else {
                putchar('.');
            }
        });
        bus.waitForIdle();
        printf("\r\n# scan complete %s\r\n", describeStatus(bus.getStatus()));
    }

    auto run() -> void
    {
        while (true) {
            _delay_ms(measurementIntervalMsec);

            auto r = sensor.measure();
            if (r) {
                printMeasurent(r.getValue());
            } else {
                printf("# Measure...%s\r\n", describeStatus(r.getError()));
            }
        }
    }

    auto printMeasurent(const AHT20::Measurement &m) -> void
    {
        printf("Temperature = ");
        printFixedPoint(m.temperature);
        printf("    Humidity = ");
        printFixedPoint(m.humidity);
        printf("\r\n");
    }
};

auto appMain() -> void
{
    BoardConfig::setupPinmux();
    led.setLow();

    AvrI2cController bus;
    constexpr auto   config = AvrI2c::configureBitrate(F_CPU, 40000);
    static_assert(config.isValid);
    bus.apply(config);

    Sys::enableInterrupts();

    _delay_ms(500);

    AHT20App app {bus};
    app.scan();
    app.start();
    app.run();
}
