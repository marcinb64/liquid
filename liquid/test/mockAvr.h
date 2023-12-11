#ifndef MOCKAVR_H_
#define MOCKAVR_H_

#include <stdint.h>

static constexpr auto F_CPU = 16'000'000;

extern uint8_t mock_mem[1024];

auto mockMemReset() -> void;
auto memAt(uint16_t addr) -> int;
auto writeMemAt(uint16_t addr) -> uint8_t &;

#endif
