#pragma once

#include <cstdint>

typedef uint64_t B64; // 64 bit board

// the things I do for readability... (aren't very readable themselves eh)
enum TileIndex {
	A1_index = 0,
	B1_index,
	C1_index,
	D1_index,
	E1_index,
	F1_index,
	G1_index,
	H1_index,
	A2_index = 8,
	B2_index,
	C2_index,
	D2_index,
	E2_index,
	F2_index,
	G2_index,
	H2_index,
	A3_index = 16,
	B3_index,
	C3_index,
	D3_index,
	E3_index,
	F3_index,
	G3_index,
	H3_index,
	A4_index = 24,
	B4_index,
	C4_index,
	D4_index,
	E4_index,
	F4_index,
	G4_index,
	H4_index,
	A5_index = 32,
	B5_index,
	C5_index,
	D5_index,
	E5_index,
	F5_index,
	G5_index,
	H5_index,
	A6_index = 40,
	B6_index,
	C6_index,
	D6_index,
	E6_index,
	F6_index,
	G6_index,
	H6_index,
	A7_index = 48,
	B7_index,
	C7_index,
	D7_index,
	E7_index,
	F7_index,
	G7_index,
	H7_index,
	A8_index = 56,
	B8_index,
	C8_index,
	D8_index,
	E8_index,
	F8_index,
	G8_index,
	H8_index
};

// shift each tile to its index
constexpr B64 A1 = (1ULL << A1_index);
constexpr B64 B1 = (1ULL << B1_index);
constexpr B64 C1 = (1ULL << C1_index);
constexpr B64 D1 = (1ULL << D1_index);
constexpr B64 E1 = (1ULL << E1_index);
constexpr B64 F1 = (1ULL << F1_index);
constexpr B64 G1 = (1ULL << G1_index);
constexpr B64 H1 = (1ULL << H1_index);
constexpr B64 A2 = (1ULL << A2_index);
constexpr B64 B2 = (1ULL << B2_index);
constexpr B64 C2 = (1ULL << C2_index);
constexpr B64 D2 = (1ULL << D2_index);
constexpr B64 E2 = (1ULL << E2_index);
constexpr B64 F2 = (1ULL << F2_index);
constexpr B64 G2 = (1ULL << G2_index);
constexpr B64 H2 = (1ULL << H2_index);
constexpr B64 A3 = (1ULL << A3_index);
constexpr B64 B3 = (1ULL << B3_index);
constexpr B64 C3 = (1ULL << C3_index);
constexpr B64 D3 = (1ULL << D3_index);
constexpr B64 E3 = (1ULL << E3_index);
constexpr B64 F3 = (1ULL << F3_index);
constexpr B64 G3 = (1ULL << G3_index);
constexpr B64 H3 = (1ULL << H3_index);
constexpr B64 A4 = (1ULL << A4_index);
constexpr B64 B4 = (1ULL << B4_index);
constexpr B64 C4 = (1ULL << C4_index);
constexpr B64 D4 = (1ULL << D4_index);
constexpr B64 E4 = (1ULL << E4_index);
constexpr B64 F4 = (1ULL << F4_index);
constexpr B64 G4 = (1ULL << G4_index);
constexpr B64 H4 = (1ULL << H4_index);
constexpr B64 A5 = (1ULL << A5_index);
constexpr B64 B5 = (1ULL << B5_index);
constexpr B64 C5 = (1ULL << C5_index);
constexpr B64 D5 = (1ULL << D5_index);
constexpr B64 E5 = (1ULL << E5_index);
constexpr B64 F5 = (1ULL << F5_index);
constexpr B64 G5 = (1ULL << G5_index);
constexpr B64 H5 = (1ULL << H5_index);
constexpr B64 A6 = (1ULL << A6_index);
constexpr B64 B6 = (1ULL << B6_index);
constexpr B64 C6 = (1ULL << C6_index);
constexpr B64 D6 = (1ULL << D6_index);
constexpr B64 E6 = (1ULL << E6_index);
constexpr B64 F6 = (1ULL << F6_index);
constexpr B64 G6 = (1ULL << G6_index);
constexpr B64 H6 = (1ULL << H6_index);
constexpr B64 A7 = (1ULL << A7_index);
constexpr B64 B7 = (1ULL << B7_index);
constexpr B64 C7 = (1ULL << C7_index);
constexpr B64 D7 = (1ULL << D7_index);
constexpr B64 E7 = (1ULL << E7_index);
constexpr B64 F7 = (1ULL << F7_index);
constexpr B64 G7 = (1ULL << G7_index);
constexpr B64 H7 = (1ULL << H7_index);
constexpr B64 A8 = (1ULL << A8_index);
constexpr B64 B8 = (1ULL << B8_index);
constexpr B64 C8 = (1ULL << C8_index);
constexpr B64 D8 = (1ULL << D8_index);
constexpr B64 E8 = (1ULL << E8_index);
constexpr B64 F8 = (1ULL << F8_index);
constexpr B64 G8 = (1ULL << G8_index);
constexpr B64 H8 = (1ULL << H8_index);
