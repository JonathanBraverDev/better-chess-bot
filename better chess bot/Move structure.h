#pragma once

#include "Board structure.h"
#include "Position structure.h"
#include "Piece structure.h"

enum MoveType {
    NORMAL = 0,
    CASTLE_LONG,
    CASTLE_SHORT,
    GOOD_GENERIC,
    CHECK = 4,
    CHECK_CASTLE_LONG,
    CHECK_CASTLE_SHORT,
    CAPTURE = 8,
    CAPTURE_GOOD_GENERIC = 11,
    CAPTURE_CHECK,
    CAPTURE_CHECK_GOOD_GENERIC = 15,
    PROMOTION = 16,
    PROMOTION_GOOD_GENERIC = 19,
    PROMOTION_CHECK,
    PROMOTION_CHECK_GOOD_GENERIC = 23,
    PROMOTION_CAPTURE,
    PROMOTION_CAPTURE_GOOD_GENERIC = 27,
    PROMOTION_CAPTURE_CHECK,
    PROMOTION_CAPTURE_CHECK_GOOD_GENERIC = 31,
};

// new MoveType encoding:
// promote bit
// capture bit
// check bit
// 1 unassigned
// 2 general bits:
// 00 normal
// 01, 10 - castle types
// 11 - capture up, other generic good looking move

typedef uint32_t BitMove;

struct Move {
    B64 origin;
    B64 destination; // castling rook here, offers full compaability with 960 chess, pround of that one
    Piece piece; // possibly add members directly here
    MoveType type;
    PieceType captured_type;
    PieceType promoted_type;
    B64 special_move_rigths; // avalible BEFORE the move, undoing this is such a headache i just gave up
    // do i really need to be able to undo moves at all?
};

// new Move encoding:
// 6 bit origin index - must
// 6 bit destination index - must
// 1 bit mover color - logic help 
// 3 bit moving/promoted type - must, logic help. when promote flag is on look as promotion
// 3 bit captured type - reversability, logic help
// 3 bit own castle rights - reversability
// 1 bit valid en passant - reversability, logic help
// 3 bit en passant index - reversability, logic help
// 
// total 26 bit move, leaving 6 for order hints 

inline bool is_white_player(const GameState& state) { return (state.turn % 2 == 0); };
inline Move invert_move(Move move) { 
    swap(move.origin, move.destination);
    return move; 
}

Move expand_move(const SidedPosition& sided_position, const BitMove bitmove);

constexpr int ORIGIN_INDEX_LENGTH = 6;
constexpr int DESTINATION_INDEX_LENGTH = 6;
constexpr int PROMOTE_LENGTH = 2;
constexpr int CAPTURE_LENGTH = 3;
constexpr int WHITE_CASTLE_LENGTH = 3;
constexpr int BLACK_CASTLE_LENGTH = 3;
constexpr int EN_PASSANT_VALID_LENGTH = 1;
constexpr int EN_PASSANT_INDEX_LENGTH = 3;
constexpr int MOVE_TYPE_LENGTH = 5;
// sanity check
constexpr int TOTAL_LENGTH = ORIGIN_INDEX_LENGTH + DESTINATION_INDEX_LENGTH + PROMOTE_LENGTH + CAPTURE_LENGTH + WHITE_CASTLE_LENGTH +
                             BLACK_CASTLE_LENGTH + EN_PASSANT_VALID_LENGTH + EN_PASSANT_INDEX_LENGTH + MOVE_TYPE_LENGTH;

constexpr int ORIGIN_INDEX_OFFSET = 0;
constexpr int DESTINATION_INDEX_OFFSET = ORIGIN_INDEX_OFFSET + ORIGIN_INDEX_LENGTH;
constexpr int PROMOTE_OFFSET = DESTINATION_INDEX_OFFSET + DESTINATION_INDEX_LENGTH;
constexpr int CAPTURE_OFFSET = PROMOTE_OFFSET + PROMOTE_LENGTH;
constexpr int WHITE_CASTLE_OFFSET = CAPTURE_OFFSET + CAPTURE_LENGTH;
constexpr int BLACK_CASTLE_OFFSET = WHITE_CASTLE_OFFSET + WHITE_CASTLE_LENGTH;
constexpr int EN_PASSANT_VALID_OFFSET = BLACK_CASTLE_OFFSET + BLACK_CASTLE_LENGTH;
constexpr int EN_PASSANT_INDEX_OFFSET = EN_PASSANT_VALID_OFFSET + EN_PASSANT_VALID_LENGTH;
constexpr int MOVE_TYPE_OFFSET = EN_PASSANT_INDEX_OFFSET + EN_PASSANT_INDEX_LENGTH;
// sanity check
constexpr int TOTAL_OFFSET = MOVE_TYPE_OFFSET + MOVE_TYPE_LENGTH;

constexpr BitMove ORIGIN_INDEX_MASK = 0b111111 << ORIGIN_INDEX_OFFSET;
constexpr BitMove DESTINATION_INDEX_MASK = 0b111111 << DESTINATION_INDEX_OFFSET;
constexpr BitMove PROMOTE_MASK = 0b11 << PROMOTE_OFFSET;
constexpr BitMove CAPTURE_MASK = 0b111 << CAPTURE_OFFSET;
constexpr BitMove WHITE_CASTLE_MASK = 0b111 << WHITE_CASTLE_OFFSET;
constexpr BitMove BLACK_CASTLE_MASK = 0b111 << BLACK_CASTLE_OFFSET;
constexpr BitMove EN_PASSANT_VALID_MASK = 0b1 << EN_PASSANT_VALID_OFFSET;
constexpr BitMove EN_PASSANT_INDEX_MASK = 0b111 << EN_PASSANT_INDEX_OFFSET;

constexpr BitMove MOVE_TYPE_MASK = 0b11111 << MOVE_TYPE_OFFSET; // no mask needed with new MoveType format, compare full int value
constexpr BitMove IS_PROMOTE_MASK = 0b10000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CAPTURE_MASK = 0b01000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CHECK_MASK = 0b00100 << MOVE_TYPE_OFFSET;
constexpr BitMove MISC_MOVE_TYPE_MASK = 0b00011 << MOVE_TYPE_OFFSET;

// sanity check
constexpr BitMove MOVE_TYPE_MASK_CHECK = IS_PROMOTE_MASK | IS_CAPTURE_MASK | IS_CHECK_MASK | MISC_MOVE_TYPE_MASK;
constexpr BitMove TOTAL_MASK = ORIGIN_INDEX_MASK + DESTINATION_INDEX_MASK + PROMOTE_MASK + CAPTURE_MASK + WHITE_CASTLE_MASK +
                               BLACK_CASTLE_MASK + EN_PASSANT_VALID_MASK + EN_PASSANT_INDEX_MASK + MOVE_TYPE_MASK;
