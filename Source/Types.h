#pragma once

#include <cstdint>
#include <string>

typedef uint64_t B64;          // 64 bit board
typedef uint32_t BitMove;      // bit-encoded move format
typedef uint8_t BitRights;     // minimalistic tracing of special move rights
typedef uint8_t BoardIndex;    // 0-63 index of a tile
typedef std::string FenString; // FEN string representation of a position
