#if defined LIQUID_BOARD_ArduinoNano
#include "boards/ArduinoNano.h"
#elif defined LIQUID_BOARD_ArduinoMega
#include "boards/ArduinoMega.h"
#else
#error "Unknown board, check LIQUID_BOARD"
#endif
