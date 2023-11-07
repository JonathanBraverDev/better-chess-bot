#pragma once

#include "Types.h"

constexpr int ORIGIN_INDEX_LENGTH = 6;
constexpr int DESTINATION_INDEX_LENGTH = 6;
constexpr int IS_WHITE_MOVE_LENGTH = 1;
constexpr int MOVING_TYPE_LENGTH = 3;
constexpr int CAPTURED_TYPE_LENGTH = 3;
constexpr int UNUSED_SPACE_LENGTH = 8;
constexpr int MOVE_TYPE_LENGTH = 5;
// sanity check
constexpr int TOTAL_LENGTH = ORIGIN_INDEX_LENGTH + DESTINATION_INDEX_LENGTH + IS_WHITE_MOVE_LENGTH + MOVING_TYPE_LENGTH +
CAPTURED_TYPE_LENGTH + UNUSED_SPACE_LENGTH + MOVE_TYPE_LENGTH;

constexpr int ORIGIN_INDEX_OFFSET = 0;
constexpr int DESTINATION_INDEX_OFFSET = ORIGIN_INDEX_OFFSET + ORIGIN_INDEX_LENGTH;
constexpr int IS_WHITE_MOVE_OFFSET = DESTINATION_INDEX_OFFSET + DESTINATION_INDEX_LENGTH;
constexpr int MOVING_TYPE_OFFSET = IS_WHITE_MOVE_OFFSET + IS_WHITE_MOVE_LENGTH;
constexpr int CAPTURED_TYPE_OFFSET = MOVING_TYPE_OFFSET + MOVING_TYPE_LENGTH;
constexpr int UNUSED_SPACE_OFFSET = CAPTURED_TYPE_OFFSET + CAPTURED_TYPE_LENGTH;
constexpr int MOVE_TYPE_OFFSET = UNUSED_SPACE_OFFSET + UNUSED_SPACE_LENGTH;
// sanity check
constexpr int TOTAL_OFFSET = MOVE_TYPE_OFFSET + MOVE_TYPE_LENGTH;

constexpr BitMove ORIGIN_INDEX_MASK = 0b111111 << ORIGIN_INDEX_OFFSET;
constexpr BitMove DESTINATION_INDEX_MASK = 0b111111 << DESTINATION_INDEX_OFFSET;
constexpr BitMove IS_WHITE_MOVE_MASK = 0b1 << IS_WHITE_MOVE_OFFSET;
constexpr BitMove MOVING_TYPE_MASK = 0b111 << MOVING_TYPE_OFFSET;
constexpr BitMove CAPTURED_TYPE_MASK = 0b111 << CAPTURED_TYPE_OFFSET;
constexpr BitMove UNUSED_SPACE_MASK = 0b11111111 << CAPTURED_TYPE_OFFSET;
constexpr BitMove MOVE_TYPE_MASK = 0b11111 << MOVE_TYPE_OFFSET;
// sanity check
constexpr BitMove TOTAL_MASK = ORIGIN_INDEX_MASK | DESTINATION_INDEX_MASK | IS_WHITE_MOVE_MASK |
MOVING_TYPE_MASK | CAPTURED_TYPE_MASK | UNUSED_SPACE_MASK | MOVE_TYPE_MASK;

constexpr BitMove IS_PROMOTE_MASK = 0b10000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CHECK_MASK = 0b01000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CAPTURE_MASK = 0b00100 << MOVE_TYPE_OFFSET;
constexpr BitMove MISC_MOVE_TYPE_MASK = 0b00011 << MOVE_TYPE_OFFSET;
// sanity check
constexpr BitMove MOVE_TYPE_MASK_CHECK = IS_PROMOTE_MASK | IS_CHECK_MASK |
IS_CAPTURE_MASK | MISC_MOVE_TYPE_MASK;


// BitMove encoding:
// 6 bit origin index - must
// 6 bit destination index - must
// 1 bit mover color - logic help (redundant with position indicator?)
// 3 bit moving/promoted type - must, logic help. when promote flag is on look as promotion
// 3 bit captured type - logic help
// total 19 bit move
// 
// note that everything but the special move right is 100% reversibe as is
// 
// Move type / order hints
// 1 bit promotion flag
// 1 bit check flag
// 1 bit capture flag
// 2 bit misc move types (normal, both castles, generic good move)
// total 5 order bits
// 
// 8 bits left unused for now (possibly BitRights)

// BitRights encoding:
// 2 bit L/R castle white
// 2 bit L/R castle black
// 1 bit valid en passant
// 3 bit en passant index
// all avalible before the move