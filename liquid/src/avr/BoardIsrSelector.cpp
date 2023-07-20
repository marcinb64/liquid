#if defined LIQUID_BOARD_ArduinoNano
#include "boards/ArduinoNanoIsr.cpp"
#elif defined LIQUID_BOARD_ArduinoMega
#include "boards/ArduinoMegaIsr.cpp"
#else
#error "Unknown board, check LIQUID_BOARD"
#endif
