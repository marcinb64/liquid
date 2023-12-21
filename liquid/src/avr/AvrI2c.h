#ifndef LIQUID_AVRI2C_H_
#define LIQUID_AVRI2C_H_

#include <AvrInterrupts.h>
#include <Interrupts.h>
#include <Reg.h>
#include <util.h>

#include <assert.h>
#include <math.h>

namespace liquid
{

class AvrI2c
{
public:
    using ReadyCallback = IrqHandler;
    using ScanCallback = auto(*)(uint8_t address, bool present) -> void;

    static constexpr uint16_t base = 0xB8;

    static constexpr auto TWBR_addr() -> uint16_t { return base + 0; };
    static constexpr auto TWSR_addr() -> uint16_t { return base + 1; };
    static constexpr auto TWAR_addr() -> uint16_t { return base + 2; };
    static constexpr auto TWDR_addr() -> uint16_t { return base + 3; };
    static constexpr auto TWCR_addr() -> uint16_t { return base + 4; };
    static constexpr auto TWAMR_addr() -> uint16_t { return base + 5; };

    enum class Status {
        Ok,
        InProgress,
        Nack,
        ArbitrationLost,
        BusError,
        Unknown,
    };

    enum class Prescaler {
        Div1 = 0,
        Div4 = 1,
        Div16 = 2,
        Div64 = 3,
    };

    struct BitrateConfig;

    static constexpr auto getSclFrequency(unsigned long cpuFreq, uint8_t twbr, Prescaler prescaler)
        -> float
    {
        const auto prescalerVal = static_cast<int>(prescaler);
        return static_cast<float>(cpuFreq) /
               static_cast<float>(16 + 2 * twbr * (1 << (2 * prescalerVal)));
    }

    static constexpr Prescaler allPrescalers[] = {Prescaler::Div1, Prescaler::Div4,
                                                  Prescaler::Div16, Prescaler::Div64};

    static constexpr auto configureBitrate(unsigned long cpuFreq, float bitrate) -> BitrateConfig
    {
        for (int i = 0; i < 4; ++i) {
            const auto r = configureBitrate(cpuFreq, static_cast<Prescaler>(i), bitrate);
            if (r.isValid) {
                return r;
            }
        }
        return {false, Prescaler::Div1, 0};
    }

    static constexpr auto configureBitrate(unsigned long cpuFreq, Prescaler prescaler,
                                           float bitrate) -> BitrateConfig
    {
        const auto  prescalerVal = static_cast<int>(prescaler);
        const auto  prescalerMult = 1 << (2 * prescalerVal);
        const float x = (static_cast<float>(cpuFreq) / bitrate - 16.0f) /
                        (2 * static_cast<float>(prescalerMult));
        if (x > 255 || x < 1) {
            return {false, Prescaler::Div1, 0};
        } else {
            return {true, prescaler, static_cast<uint8_t>(round(x))};
        }
    }

    // -----------------------------------------------------------------------------

    static constexpr auto TWCR()
    {
        struct Bits : SfrBase {
            RegBits<7> TWINT {regAddr}; // TWI Interrupt Flag
            RegBits<6> TWEA {regAddr};  // TWI Enable Acknowledge Bit
            RegBits<5> TWSTA {regAddr}; // TWI Start Condition Bit
            RegBits<4> TWSTO {regAddr}; // TWI Stop Condition Bit
            RegBits<3> TWWC {regAddr};  // TWI Write Collision Flag
            RegBits<2> TWEN {regAddr};  // TWI Enable Bit
            // bit 1 reserved
            RegBits<0> TWIE {regAddr}; // TWI Interrupt Enable
        };

        return Bits {TWCR_addr()};
    }

    static constexpr auto TWSR()
    {
        struct Bits : SfrBase {
            RegBits<3, 5> TWS {regAddr}; // TWI Status
            // bit 2 reserved
            RegBits<0, 2> TWPS {regAddr}; // TWI Prescaler Bits
        };

        return Bits {TWSR_addr()};
    }

    struct BitrateConfig {
        bool      isValid;
        Prescaler prescaler;
        uint8_t   twbr;

        constexpr bool operator==(const BitrateConfig &other) const
        {
            return isValid == other.isValid && prescaler == other.prescaler && twbr == other.twbr;
        };

        auto apply(AvrI2c &dev) const -> void
        {
            assert(isValid);
            dev.TWSR().TWPS = static_cast<uint8_t>(prescaler);
            sfr8(dev.TWBR_addr()) = twbr;
        }
    };

    auto apply(const BitrateConfig &config) { config.apply(*this); }
};

class AvrI2cController : public AvrI2c
{
public:
    AvrI2cController()
    {
        TWCR().TWINT = 0;
        TWCR().TWIE = 1;
        TWCR().TWEN = 1;
        installIrqHandler(
            Irq::Twi, IrqHandler::callMemberFunc<AvrI2cController, &AvrI2cController::isr>(this));
    }

    ~AvrI2cController()
    {
        TWCR().TWIE = 0;
        TWCR().TWEN = 0;
    }

    auto blockingWriteByte(uint8_t address, uint8_t data) -> Result<void, Status>
    {
        write(address, &data, 1);
        waitForIdle();

        if (status == Status::Ok) {
            return Result<void, Status>::ok();
        } else {
            return Result<void, Status>::err(Status {status});
        }
    }

    auto blockingWrite(uint8_t address, uint8_t *data, size_t size) -> Result<uint8_t *, Status>
    {
        write(address, data, size);
        waitForIdle();
        if (status == Status::Ok) {
            return Result<uint8_t *, Status>::ok(data);
        } else {
            return Result<uint8_t *, Status>::err(Status {status});
        }
    }

    auto write(uint8_t address, uint8_t *data, size_t size) -> void
    {
        waitForIdle();

        mode_func = &AvrI2cController::write_func;
        pendingAddress = address;
        currentData = data;
        dataSize = size;

        start();
    }

    auto blockingReadByte(uint8_t address) -> Result<uint8_t, Status>
    {
        uint8_t data {0};
        read(address, &data, 1);
        waitForIdle();

        if (status == Status::Ok) {
            return Result<uint8_t, Status>::ok(data);
        } else {
            return Result<uint8_t, Status>::err(Status {status});
        }
    }

    auto blockingRead(uint8_t address, uint8_t *data, size_t size) -> Result<uint8_t *, Status>
    {
        read(address, data, size);
        waitForIdle();
        if (status == Status::Ok) {
            return Result<uint8_t *, Status>::ok(data);
        } else {
            return Result<uint8_t *, Status>::err(Status {status});
        }
    }

    auto read(uint8_t address, uint8_t *data, size_t size) -> void
    {
        waitForIdle();

        mode_func = &AvrI2cController::read_func;
        pendingAddress = address;
        currentData = data;
        dataSize = size;

        start();
    }

    auto probe(uint8_t address)
    {
        waitForIdle();

        mode_func = &AvrI2cController::probe_func;
        pendingAddress = address;
        currentData = nullptr;
        dataSize = 0;

        start();
    }

    auto scan(ScanCallback callback)
    {
        waitForIdle();

        scanCallback = callback;
        mode_func = &AvrI2cController::scan_func;
        pendingAddress = 0;
        currentData = 0;
        dataSize = 0;

        start();
    }

    auto waitForIdle() -> Status
    {
        while (status == Status::InProgress)
            ;

        return status;
    }

    auto getStatus() const { return status; }

    auto onReady(ReadyCallback cb) -> void { readyCallback = cb; }

    auto isr() -> void
    {
        lastStatusCode = readStatus();
        (this->*mode_func)(lastStatusCode);
        TWCR().TWINT = 1; // clear interrupt flag
    }

private:
    enum class Mode { Read, Write };

    // The data sheet specifies the status codes as values of TWSR register,
    // where the status is on bits 7-3.
    struct StatusCode {
        static constexpr uint8_t BusError = 0x00;
        static constexpr uint8_t StartTxd = 0x08 >> 3;
        static constexpr uint8_t RepeatedStartTxd = 0x10 >> 3;
        static constexpr uint8_t WriteAddressAckRxd = 0x18 >> 3;
        static constexpr uint8_t WriteAddressNackRxd = 0x20 >> 3;
        static constexpr uint8_t DataAckRxd = 0x28 >> 3;
        static constexpr uint8_t DataNackRxd = 0x30 >> 3;
        static constexpr uint8_t ArbitrationLost = 0x38 >> 3;
        static constexpr uint8_t ReadAddressAckRxd = 0x40 >> 3;
        static constexpr uint8_t ReadAddressNackRxd = 0x48 >> 3;
        static constexpr uint8_t DataRxdWillAck = 0x50 >> 3;
        static constexpr uint8_t DataRxdWillNack = 0x58 >> 3;
    };

    volatile uint8_t  pendingAddress {0};
    volatile uint8_t *currentData {nullptr};
    volatile size_t   dataSize {0};
    volatile uint8_t  lastStatusCode {0};
    volatile Status   status {Status::Ok};
    volatile Status   pendingStatus {Status::Ok};
    ReadyCallback     readyCallback {[](void *) {}, nullptr};
    ScanCallback      scanCallback {[](uint8_t, bool) {}};

    void (AvrI2cController::*mode_func)(uint8_t) = &AvrI2cController::idle;

    auto idle(uint8_t) -> void
    {
        assert(0); // The interrupt should never happen when in idle state
    }

    auto handleStart(Mode mode) -> void
    {
        writeAddr(pendingAddress, mode);
        TWCR().TWSTA = 0;
    }

    auto probe_func(uint8_t statusCode) -> void
    {
        switch (statusCode) {
        case StatusCode::StartTxd:
        case StatusCode::RepeatedStartTxd: handleStart(Mode::Write); break;
        case StatusCode::WriteAddressAckRxd: finish(Status::Ok); break;
        case StatusCode::WriteAddressNackRxd: finish(Status::Nack); break;
        default: finish(codeToStatus(statusCode)); break;
        }
    }

    auto scan_func(uint8_t statusCode) -> void
    {
        switch (statusCode) {
        case StatusCode::StartTxd:
        case StatusCode::RepeatedStartTxd: handleStart(Mode::Write); break;
        case StatusCode::WriteAddressAckRxd:
            scanCallback(pendingAddress, true);
            if (++pendingAddress <= 127) {
                TWCR().TWSTA = 1;
                break;
            } else {
                finish(Status::Ok);
            }
            break;
        case StatusCode::WriteAddressNackRxd:
            scanCallback(pendingAddress, false);
            if (++pendingAddress <= 127) {
                TWCR().TWSTA = 1;
                break;
            } else {
                finish(Status::Ok);
            }
            break;
        default: finish(codeToStatus(statusCode)); break;
        }
    }

    auto write_func(uint8_t statusCode) -> void
    {
        switch (statusCode) {
        case StatusCode::StartTxd:
        case StatusCode::RepeatedStartTxd: handleStart(Mode::Write); break;
        case StatusCode::WriteAddressAckRxd: writeData(*currentData++); break;
        case StatusCode::WriteAddressNackRxd: finish(Status::Nack); break;
        case StatusCode::DataAckRxd:
            if (--dataSize != 0) {
                writeData(*currentData++);
            } else {
                finish(Status::Ok);
            }
            break;
        case StatusCode::DataNackRxd: finish(Status::Nack); break;
        default: finish(codeToStatus(statusCode)); break;
        }
    }

    auto read_func(uint8_t statusCode) -> void
    {
        switch (statusCode) {
        case StatusCode::StartTxd:
        case StatusCode::RepeatedStartTxd: handleStart(Mode::Read); break;
        case StatusCode::ReadAddressAckRxd:
            if (dataSize == 1) {
                TWCR().TWEA = 0;
            }
            break;
        case StatusCode::ReadAddressNackRxd: finish(Status::Nack); break;
        case StatusCode::DataRxdWillAck:
            *currentData++ = readData();
            if (--dataSize <= 1) {
                TWCR().TWEA = 0;
            }
            break;
        case StatusCode::DataRxdWillNack:
            if (dataSize > 0) {
                *currentData++ = readData();
            }
            finish(Status::Ok);
            break;
        default: finish(codeToStatus(statusCode)); break;
        }
    }

    auto wait_for_stop_func(uint8_t) -> void
    {
        status = pendingStatus;
        pendingStatus = Status::Unknown;
        mode_func = &AvrI2cController::idle;
        readyCallback();
    }

    auto start() -> void
    {
        status = Status::InProgress;
        pendingStatus = Status::Unknown;
        lastStatusCode = 0;

        TWCR().TWEA = 1;
        TWCR().TWSTA = 1;
        TWCR().TWINT = 1;
    }

    auto finish(Status status_) -> void
    {
        // Setting TWSTO will send the stop bit. After it has been sent,
        // TWINT interrupt will trigger one last time. Only then the operation
        // is finished and the device is ready to start another one.
        pendingStatus = status_;
        mode_func = &AvrI2cController::wait_for_stop_func;
        TWCR().TWSTA = 0;
        TWCR().TWSTO = 1;
    }

    auto codeToStatus(uint8_t statusCode) -> Status
    {
        switch (statusCode) {
        case StatusCode::ArbitrationLost: return Status::ArbitrationLost;
        case StatusCode::BusError: return Status::BusError;
        case StatusCode::DataNackRxd:
        case StatusCode::ReadAddressNackRxd:
        case StatusCode::WriteAddressNackRxd: return Status::Nack; break;
        default: return Status::Unknown;
        }
    }

    auto readStatus() const -> uint8_t { return static_cast<uint8_t>(TWSR().TWS); }

    auto writeAddr(uint8_t addr, Mode m) -> void
    {
        sfr8(TWDR_addr()) = static_cast<uint8_t>((addr << 1) | (m == Mode::Read ? 1 : 0));
    }

    auto writeData(uint8_t data) -> void { sfr8(TWDR_addr()) = data; }

    auto readData() const -> uint8_t { return sfr8(TWDR_addr()); }
};

} // namespace liquid

#endif
