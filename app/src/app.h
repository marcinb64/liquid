#include <Sys.h>
#include <SysTimer.h>

extern auto appMain() -> void;

template <class App> auto runApp()
{
    App app;
    liquid::Sys::enableInterrupts();

    while (true) {
        app.tick();
    }
}