#include "app.h"
#include <avr/BoardSelector.h>

#include <stdio.h>
#include <unistd.h>
#include <util/delay.h>
#include <string.h>

using namespace liquid;
using Board = liquid::ArduinoMega;

constexpr static auto DEBUG = false;
constexpr static auto HEARTBEAT = false;

struct BoardConfig {
    Gpio led = Board::makeGpio(Board::Gpio::BuiltInLed);
    Gpio wave = Board::makeGpio(Board::Gpio::D5);

    void setupPinmux()
    {
        led.asOutput();
        wave.asOutput();
    }
};

static BoardConfig boardConfig;
static auto        waveGen = Board::makeSquareWave(Board::Gpio::D5);
static SysTimer    sysTimer;

/* -------------------------------------------------------------------------- */

class App
{
public:
    App()
    {
        setInitialState();

        auto t = Board::makeTimer(Timer16::Timer5);
        sysTimer.setupWith(t, F_CPU, 1000.0f);
        
        waveGen.setFrequency(F_CPU, 300);
    }

    auto triggerEvent() -> void { ++event; }

    auto readLine(char *rxbuf, int size) -> int
    {
        for (int i = 0; i < size + 1; ++i) {
            char ch = (char) getchar();
            rxbuf[i] = ch;
            if (ch == '\n' || ch == '\r') {
                rxbuf[i] = 0;
                return i;
            }
        }
        return size;
    }

    auto tick() -> void
    {
        _delay_ms(10);

        // compoments tick

        // poll inputs
        int f = 0;
        printf(">");
        char cmd[32] = { 0 };
        readLine(cmd, sizeof(cmd));

        if (strcmp(cmd, "t") == 0) {
            printf("t = %lu\r\n", sysTimer.getTime());
            return;
        }

        sscanf(cmd, "%d", &f);
        printf("\r\n");

        if (f != 0) {
            if (waveGen.setFrequency(F_CPU, f))
                printf("OK\r\n");
            else
                printf("out of range\n\r");
        } else {
            printf("?\r\n");
        }

        // update state

        // update outputs

        if constexpr (DEBUG) debugPrintState();

        if constexpr (HEARTBEAT) {
            static auto beat = false;
            beat = !beat;
            if (beat)
                boardConfig.led.setHigh();
            else
                boardConfig.led.setLow();
        }
    }

private:
    SquareWave *wave;

    volatile int event = 0;

    auto setInitialState() -> void { boardConfig.led.setLow(); }
    auto debugPrintState() -> void { printf("tick\r\n"); }
};

auto boo() -> void
{
    auto w = Board::makeSquareWave(Board::Gpio::D5);
    auto cfg = w.tryConfigureFrequency(F_CPU, 444);
    if (cfg) w.apply(cfg);
}

auto appMain() -> void
{
    boo();
    boardConfig.setupPinmux();
    runApp<App>();
}
