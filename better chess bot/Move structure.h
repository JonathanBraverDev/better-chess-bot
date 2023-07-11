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


typedef uint32_t BitMove;
typedef uint8_t BitRights;

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

// BitRights encoding:
// 2 bit L/R castle white
// 2 bit L/R castle black
// 1 bit valid en passant
// 3 bit en passant index
// all avalible before the move

// BitMove encoding:
// 6 bit origin index - must
// 6 bit destination index - must
// 1 bit mover color - logic help 
// 3 bit moving/promoted type - must, logic help. when promote flag is on look as promotion
// 3 bit captured type - logic help
// potential for cutting, but no idea what to do with so many bits sooo they stay
// 2 bit own castle rights L/R - reversability
// 1 bit valid en passant - reversability, logic help
// 3 bit en passant index - reversability, logic help
// 
// total 25 bit move, leaving 7 for order hints 
// 1 bit promotion flag
// 1 bit check flag
// 1 bit capture flag
// 2 bit UNASSIGNED SPACE - no idea what to put here
// 2 bit misc move types (normal, both castles, generic good move
// this is NOT the minimal possiblbe but a generic form for all possible function i might need

// draw reset can be interptered from any pawn moves or any capture

inline bool is_white_player(const GameState& state) { return (state.turn % 2 == 0); };
inline Move invert_move(Move move) { 
    swap(move.origin, move.destination);
    return move; 
}

Move expand_move(const SidedPosition& sided_position, const BitMove bitmove);

constexpr int ORIGIN_INDEX_LENGTH = 6;
constexpr int DESTINATION_INDEX_LENGTH = 6;
constexpr int MOVING_COLOR_LENGTH = 1;
constexpr int MOVING_TYPE_LENGTH = 3;
constexpr int CAPTURED_TYPE_LENGTH = 3;
constexpr int OWN_CASTLE_RIGHTS_LENGTH = 2;
constexpr int EN_PASSANT_VALID_LENGTH = 1;
constexpr int EN_PASSANT_INDEX_LENGTH = 3;
constexpr int MOVE_TYPE_LENGTH = 7;
// sanity check
constexpr int TOTAL_LENGTH = ORIGIN_INDEX_LENGTH + DESTINATION_INDEX_LENGTH + MOVING_COLOR_LENGTH + MOVING_TYPE_LENGTH + CAPTURED_TYPE_LENGTH +
                             OWN_CASTLE_RIGHTS_LENGTH + EN_PASSANT_VALID_LENGTH + EN_PASSANT_INDEX_LENGTH + MOVE_TYPE_LENGTH;

constexpr int ORIGIN_INDEX_OFFSET = 0;
constexpr int DESTINATION_INDEX_OFFSET = ORIGIN_INDEX_OFFSET + ORIGIN_INDEX_LENGTH;
constexpr int MOVING_COLOR_OFFSET = DESTINATION_INDEX_OFFSET + DESTINATION_INDEX_LENGTH;
constexpr int MOVING_TYPE_OFFSET = MOVING_COLOR_OFFSET + MOVING_COLOR_LENGTH;
constexpr int CAPTURED_TYPE_OFFSET = MOVING_TYPE_OFFSET + MOVING_TYPE_LENGTH;
constexpr int OWN_CASTLE_RIGHTS_OFFSET = CAPTURED_TYPE_OFFSET + CAPTURED_TYPE_LENGTH;
constexpr int EN_PASSANT_VALID_OFFSET = OWN_CASTLE_RIGHTS_OFFSET + OWN_CASTLE_RIGHTS_LENGTH;
constexpr int EN_PASSANT_INDEX_OFFSET = EN_PASSANT_VALID_OFFSET + EN_PASSANT_VALID_LENGTH;
constexpr int MOVE_TYPE_OFFSET = EN_PASSANT_INDEX_OFFSET + EN_PASSANT_INDEX_LENGTH;
// sanity check
constexpr int TOTAL_OFFSET = MOVE_TYPE_OFFSET + MOVE_TYPE_LENGTH;

constexpr BitMove ORIGIN_INDEX_MASK = 0b111111 << ORIGIN_INDEX_OFFSET;
constexpr BitMove DESTINATION_INDEX_MASK = 0b111111 << DESTINATION_INDEX_OFFSET;
constexpr BitMove MOVING_COLOR_MASK = 0b1 << MOVING_COLOR_OFFSET;
constexpr BitMove MOVING_TYPE_MASK = 0b111 << MOVING_TYPE_OFFSET;
constexpr BitMove CAPTURED_TYPE_MASK = 0b111 << CAPTURED_TYPE_OFFSET;
constexpr BitMove OWN_CASTLE_RIGHTS_MASK = 0b11 << OWN_CASTLE_RIGHTS_OFFSET;
constexpr BitMove EN_PASSANT_VALID_MASK = 0b1 << EN_PASSANT_VALID_OFFSET;
constexpr BitMove EN_PASSANT_INDEX_MASK = 0b111 << EN_PASSANT_INDEX_OFFSET;
constexpr BitMove MOVE_TYPE_MASK = 0b1111111 << MOVE_TYPE_OFFSET;
// sanity check
constexpr BitMove TOTAL_MASK = ORIGIN_INDEX_MASK | DESTINATION_INDEX_MASK | MOVING_COLOR_MASK | MOVING_TYPE_MASK | CAPTURED_TYPE_MASK |
                  OWN_CASTLE_RIGHTS_MASK | EN_PASSANT_VALID_MASK | EN_PASSANT_INDEX_MASK | MOVE_TYPE_MASK;

constexpr BitMove MOVE_TYPE_MASK = 0b1111111 << MOVE_TYPE_OFFSET; // no mask needed with BitMove format, compare full int value
constexpr BitMove IS_PROMOTE_MASK = 0b1000000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CHECK_MASK = 0b0100000 << MOVE_TYPE_OFFSET;
constexpr BitMove IS_CAPTURE_MASK = 0b0010000 << MOVE_TYPE_OFFSET;
constexpr BitMove UNUSED_SPACE_MASK = 0b0001100 << MOVE_TYPE_OFFSET;
constexpr BitMove MISC_MOVE_TYPE_MASK = 0b0000011 << MOVE_TYPE_OFFSET;
// sanity check
constexpr BitMove MOVE_TYPE_MASK_CHECK = IS_PROMOTE_MASK | IS_CHECK_MASK  | IS_CAPTURE_MASK | UNUSED_SPACE_MASK | MISC_MOVE_TYPE_MASK;