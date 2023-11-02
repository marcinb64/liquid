#if defined LIQUID_BOARD_ArduinoNano

#include "boards/ArduinoNano.h"
using Board = liquid::ArduinoNano;

#elif defined LIQUID_BOARD_ArduinoMega

#include "boards/ArduinoMega.h"
using Board = liquid::ArduinoMega;

#else
#error "Unknown board, check LIQUID_BOARD"
#endif
