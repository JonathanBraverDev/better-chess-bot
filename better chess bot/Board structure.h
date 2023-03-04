#pragma once

#include <cstdint>

typedef uint64_t B64; // 64 bit board

void swap(B64& a, B64& b);

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

// constexpr > const, compiletime vs runtime evaluation, less runtime -> more better.
constexpr int BOARD_SIZE = 8;

// useful for out of bounds detection
constexpr B64 COLUMN_A = A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8;
constexpr B64 COLUMN_B = B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8;
constexpr B64 COLUMN_AB = COLUMN_A | COLUMN_B;
constexpr B64 COLUMN_C = C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8;
constexpr B64 COLUMN_D = D1 | D2 | D3 | D4 | D5 | D6 | D7 | D8;
constexpr B64 COLUMN_E = E1 | E2 | E3 | E4 | E5 | E6 | E7 | E8;
constexpr B64 COLUMB_F = F1 | F2 | F3 | F4 | F5 | F6 | F7 | F8;
constexpr B64 COLUMB_G = G1 | G2 | G3 | G4 | G5 | G6 | G7 | G8;
constexpr B64 COLUMN_H = H1 | H2 | H3 | H4 | H5 | H6 | H7 | H8;
constexpr B64 COLUMN_GH = COLUMB_G | COLUMN_H;

// used for pawn first move/promotion
constexpr B64 ROW_8 = A8 | B8 | C8 | D8 | E8 | F8 | G8 | H8;
constexpr B64 ROW_7 = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
constexpr B64 ROW_6 = A6 | B6 | C6 | D6 | E6 | F6 | G6 | H6;
constexpr B64 ROW_5 = A5 | B5 | C5 | D5 | E5 | F5 | G5 | H5;
constexpr B64 ROW_4 = A4 | B4 | C4 | D4 | E4 | F4 | G4 | H4;
constexpr B64 ROW_3 = A3 | B3 | C3 | D3 | E3 | F3 | G3 | H3;
constexpr B64 ROW_2 = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
constexpr B64 ROW_1 = A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1;
constexpr B64 TILE_BLACK = 0xAAAAAAAAAAAAAAAAULL;
constexpr B64 TILE_WHITE = 0x5555555555555555ULL;

// programmatically needed splits of the board
constexpr B64 VOID_WHITE_CASTLE = ~ROW_1;
constexpr B64 VOID_BLACK_CASTLE = ~ROW_8;
constexpr B64 VOID_ALL_EN_PASSANT = ~ROW_3 | ~ROW_6;

// used mainly for board slides, defined here for clarity
constexpr int BOARD_SIDE_ADD1 = BOARD_SIZE + 1; // this honestly feel INCREDIBLY stupid but I WAS calculating that every single call so... yeah
constexpr int BOARD_SIDE_SUB1 = BOARD_SIZE - 1;
constexpr B64 COLUMN_A_INV = ~COLUMN_A;
constexpr B64 COLUMN_H_INV = ~COLUMN_H;