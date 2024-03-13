#pragma once

#include "Types.h"
#include <climits>

constexpr int ORIGIN_INDEX_LENGTH = 6;
constexpr int DESTINATION_INDEX_LENGTH = 6;
constexpr int BIT_RIGHTS_LENGTH = 8;
constexpr int PIECE_TYPE_LENGTH = 3;
constexpr int CAPTURED_TYPE_LENGTH = 3;
constexpr int MOVE_TYPE_LENGTH = 6;

static_assert(ORIGIN_INDEX_LENGTH + DESTINATION_INDEX_LENGTH + BIT_RIGHTS_LENGTH +
	PIECE_TYPE_LENGTH + CAPTURED_TYPE_LENGTH + MOVE_TYPE_LENGTH == 32, "Total move encode length is wrong");

constexpr int ORIGIN_INDEX_OFFSET = 0;
constexpr int DESTINATION_INDEX_OFFSET = ORIGIN_INDEX_OFFSET + ORIGIN_INDEX_LENGTH;
constexpr int BIT_RIGHTS_OFFSET = DESTINATION_INDEX_OFFSET + DESTINATION_INDEX_LENGTH;
constexpr int PIECE_TYPE_OFFSET = BIT_RIGHTS_OFFSET + BIT_RIGHTS_LENGTH;
constexpr int CAPTURED_TYPE_OFFSET = PIECE_TYPE_OFFSET + PIECE_TYPE_LENGTH;
constexpr int MOVE_TYPE_OFFSET = CAPTURED_TYPE_OFFSET + CAPTURED_TYPE_LENGTH;

static_assert(MOVE_TYPE_OFFSET + MOVE_TYPE_LENGTH == 32, "Total move encode offset is wrong");

constexpr BitMove ORIGIN_INDEX_MASK = 0b111111 << ORIGIN_INDEX_OFFSET;
constexpr BitMove DESTINATION_INDEX_MASK = 0b111111 << DESTINATION_INDEX_OFFSET;
constexpr BitMove BIT_RIGHTS_MASK = 0b11111111 << BIT_RIGHTS_OFFSET;
constexpr BitMove PIECE_TYPE_MASK = 0b111 << PIECE_TYPE_OFFSET;
constexpr BitMove CAPTURED_TYPE_MASK = 0b111 << CAPTURED_TYPE_OFFSET;
constexpr BitMove MOVE_TYPE_MASK = 0b111111 << MOVE_TYPE_OFFSET;

static_assert((ORIGIN_INDEX_MASK | DESTINATION_INDEX_MASK | BIT_RIGHTS_MASK |
	PIECE_TYPE_MASK | CAPTURED_TYPE_MASK | MOVE_TYPE_MASK) == UINT_MAX, "Total move encode value is wrong");

// Move type subsection
constexpr BitMove IS_PROMOTE_MASK = 0b100000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CHECK_MASK = 0b010000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CAPTURE_MASK = 0b001000 << MOVE_TYPE_OFFSET;
constexpr BitMove FREE_FLAG_MASK = 0b000100 << MOVE_TYPE_OFFSET;
constexpr BitMove MISC_MOVE_TYPE_MASK = 0b000011 << MOVE_TYPE_OFFSET;

static_assert((IS_PROMOTE_MASK | IS_CHECK_MASK | IS_CAPTURE_MASK | FREE_FLAG_MASK | MISC_MOVE_TYPE_MASK)
	== 0b11111100000000000000000000000000, "Total move encode type value is wrong");

// BitRights subsection
constexpr BitMove WHITE_CASTLE_MASK = 0b11000000 << BIT_RIGHTS_OFFSET;
constexpr BitMove BLACK_CASTLE_MASK = 0b00110000 << BIT_RIGHTS_OFFSET;
constexpr BitMove VALID_EN_PASSANT_MASK = 0b00001000 << BIT_RIGHTS_OFFSET;
constexpr BitMove EN_PASSANT_INDEX_MASK = 0b00000111 << BIT_RIGHTS_OFFSET;

static_assert((WHITE_CASTLE_MASK | BLACK_CASTLE_MASK | VALID_EN_PASSANT_MASK | EN_PASSANT_INDEX_MASK)
	== 0b00000000000011111111000000000000, "Total move encode type value is wrong");


// BitMove encoding:
// 6 bit origin index - must
// 6 bit destination index - must
// 3 bit moving/promoted type - must, logic help. when promote flag is on look as promotion
// 3 bit captured type - logic help
// total 18 bit move

// Move type flags:
// 1 bit promotion flag
// 1 bit check flag
// 1 bit capture flag
// 1 bit FREE FLAG
// 2 bit misc move types (see emun)
// total 6 order bits
// note that everything but the special move right is 100% reversibe as is

// BitRights encoding: (all avalible before the move)
// 2 bit L/R castle white
// 2 bit L/R castle black
// 1 bit valid en passant
// 3 bit en passant index
// total 8 BitRights bits
