#pragma once

#include <cstdint>

#include "Board structure.h"

uint8_t lowestBitIndex(uint32_t v);
uint8_t lowestBitIndex32_2(uint64_t v);
uint8_t lowestBitIndex64(uint64_t v);
inline B64 lowestBitBoard(B64 board) { return (board & -board); }
