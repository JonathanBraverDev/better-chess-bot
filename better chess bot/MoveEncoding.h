#pragma once

#include "Types.h"
#include <climits>

// high value bits are at the bottom of this list
// basic data
constexpr int ORIGIN_INDEX_LENGTH = 6;
constexpr int DESTINATION_INDEX_LENGTH = 6;
// bit rights
constexpr int CASTLE_RIGHTS_FLAGS = 4;
constexpr int VALID_EN_PASSANT_LENGTH = 1;
constexpr int EN_PASSANT_INDEX_LENGTH = 3;
// MVV-LVA
constexpr int PIECE_TYPE_LENGTH = 3;
constexpr int CAPTURED_TYPE_LENGTH = 3;
// move ordering flags
// todo: one bit can be saved here, see MoveType enum
constexpr int MISC_MOVE_TYPE_LENGTH = 2;
constexpr int MOVE_FLAGS_LENGTH = 4; // flags only need masks

static_assert(ORIGIN_INDEX_LENGTH + DESTINATION_INDEX_LENGTH + CASTLE_RIGHTS_FLAGS +
	VALID_EN_PASSANT_LENGTH + EN_PASSANT_INDEX_LENGTH + PIECE_TYPE_LENGTH + CAPTURED_TYPE_LENGTH +
	MISC_MOVE_TYPE_LENGTH + MOVE_FLAGS_LENGTH == 32, "Total move encode length is wrong");

constexpr int ORIGIN_INDEX_OFFSET = 0;
constexpr int DESTINATION_INDEX_OFFSET = ORIGIN_INDEX_OFFSET + ORIGIN_INDEX_LENGTH;
constexpr int CASTLE_RIGHTS_FLAGS_OFFSET = DESTINATION_INDEX_OFFSET + DESTINATION_INDEX_LENGTH;
constexpr int VALID_EN_PASSANT_OFFSET = CASTLE_RIGHTS_FLAGS_OFFSET + CASTLE_RIGHTS_FLAGS;
constexpr int EN_PASSANT_INDEX_OFFSET = VALID_EN_PASSANT_OFFSET + VALID_EN_PASSANT_LENGTH;
constexpr int PIECE_TYPE_OFFSET = EN_PASSANT_INDEX_OFFSET + EN_PASSANT_INDEX_LENGTH;
constexpr int CAPTURED_TYPE_OFFSET = PIECE_TYPE_OFFSET + PIECE_TYPE_LENGTH;
constexpr int MISC_MOVE_TYPE_OFFSET = CAPTURED_TYPE_OFFSET + CAPTURED_TYPE_LENGTH;
constexpr int MOVE_FLAGS_OFFSET = MISC_MOVE_TYPE_OFFSET + MISC_MOVE_TYPE_LENGTH;

static_assert(MOVE_FLAGS_OFFSET + MOVE_FLAGS_LENGTH == 32, "Total move encode offset is wrong");

constexpr BitMove ORIGIN_INDEX_MASK = 0b111111 << ORIGIN_INDEX_OFFSET;
constexpr BitMove DESTINATION_INDEX_MASK = 0b111111 << DESTINATION_INDEX_OFFSET;
constexpr BitMove CASTLE_RIGHTS_FLAGS_MASK = 0b1111 << CASTLE_RIGHTS_FLAGS_OFFSET;
constexpr BitMove VALID_EN_PASSANT_MASK = 0b1 << VALID_EN_PASSANT_OFFSET;
constexpr BitMove EN_PASSANT_INDEX_MASK = 0b111 << EN_PASSANT_INDEX_OFFSET;
constexpr BitMove PIECE_TYPE_MASK = 0b111 << PIECE_TYPE_OFFSET;
constexpr BitMove CAPTURED_TYPE_MASK = 0b111 << CAPTURED_TYPE_OFFSET;
constexpr BitMove MISC_MOVE_TYPE_MASK = 0b11 << MISC_MOVE_TYPE_OFFSET;
constexpr BitMove MOVE_FLAGS_MASK = 0b1111 << MOVE_FLAGS_OFFSET;

static_assert((ORIGIN_INDEX_MASK | DESTINATION_INDEX_MASK | CASTLE_RIGHTS_FLAGS_MASK |
	VALID_EN_PASSANT_MASK | EN_PASSANT_INDEX_MASK | PIECE_TYPE_MASK | CAPTURED_TYPE_MASK |
	MISC_MOVE_TYPE_MASK | MOVE_FLAGS_MASK) == UINT_MAX, "Total move encode mask is wrong");


constexpr BitMove IS_PROMOTE_MASK = 0b1000 << MOVE_FLAGS_OFFSET;
constexpr BitMove IS_CHECK_MASK = 0b0100 << MOVE_FLAGS_OFFSET;
constexpr BitMove IS_CAPTURE_MASK = 0b0010 << MOVE_FLAGS_OFFSET;
constexpr BitMove FREE_FLAG_MASK = 0b0001 << MOVE_FLAGS_OFFSET;

static_assert((IS_PROMOTE_MASK | IS_CHECK_MASK | IS_CAPTURE_MASK | FREE_FLAG_MASK)
	== 0b11110000000000000000000000000000, "Move ordering flag masks are wrong");


constexpr BitMove WHITE_SHORT_CASTLE_MASK = 0b1000 << CASTLE_RIGHTS_FLAGS_OFFSET;
constexpr BitMove WHITE_LONG_CASTLE_MASK = 0b0100 << CASTLE_RIGHTS_FLAGS_OFFSET;
constexpr BitMove BLACK_SHORT_CASTLE_MASK = 0b0010 << CASTLE_RIGHTS_FLAGS_OFFSET;
constexpr BitMove BLACK_LONG_CASTLE_MASK = 0b0001 << CASTLE_RIGHTS_FLAGS_OFFSET;

static_assert((WHITE_SHORT_CASTLE_MASK | WHITE_LONG_CASTLE_MASK | BLACK_SHORT_CASTLE_MASK | BLACK_LONG_CASTLE_MASK)
	== CASTLE_RIGHTS_FLAGS_MASK, "BitRights castle flags are wrong");


constexpr BitMove BIT_RIGHTS_MASK = 0b11111111 << CASTLE_RIGHTS_FLAGS_OFFSET;

static_assert((CASTLE_RIGHTS_FLAGS_MASK | VALID_EN_PASSANT_MASK | EN_PASSANT_INDEX_MASK)
	== BIT_RIGHTS_MASK, "BitRights mask is wrong");



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
// 1 bit FREE FLAG (currently unused)
// 2 bit misc move types (see emun)
// total 6 order bits
// note that everything but the special move rights is 100% reversibe as is

// BitRights encoding: (avalible before the move)
// 2 bit L/R castle white
// 2 bit L/R castle black
// 1 bit valid en passant
// 3 bit en passant index
// total 8 BitRights bits
