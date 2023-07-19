#pragma once

#include "Board structure.h"
#include "Position structure.h"
#include "Piece structure.h"

enum MoveType { // can it be improved? i need 2 checks to figure out the castle type and if any
    NORMAL = 0,
    CASTLE_LONG,
    CASTLE_SHORT,
    GOOD_GENERIC,
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
// 2 bit misc move types (normal, both castles, generic good move
// total 5 order bits
// 
// 8 bits left unused for now
// why undo if i can save the original and just reset every time?

// draw reset can be interptered from any pawn moves or any capture

inline bool is_white_player(const GameState& state) { return (state.turn % 2 == 0); };
inline Move invert_move(Move move) { 
    swap(move.origin, move.destination);
    return move; 
}

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

inline BitMove get_origin_index(BitMove move) { return (move & ORIGIN_INDEX_MASK) >> ORIGIN_INDEX_OFFSET; }
inline BitMove get_destination_index(BitMove move) { return (move & DESTINATION_INDEX_MASK) >> DESTINATION_INDEX_OFFSET; }
inline B64 origin_board(BitMove move) { return bit_board_from_index(get_origin_index(move)); }
inline B64 destination_board(BitMove move) { return bit_board_from_index(get_destination_index(move)); }

inline bool is_moving_white(BitMove move) { return (move & IS_WHITE_MOVE_MASK); }
inline BitMove get_moving_type(BitMove move) { return (move & MOVING_TYPE_MASK) >> MOVING_TYPE_OFFSET; }
inline BitMove get_captured_type(BitMove move) { return (move & CAPTURED_TYPE_MASK) >> CAPTURED_TYPE_OFFSET; }
inline BitMove get_unused_space(BitMove move) { return (move & UNUSED_SPACE_MASK) >> UNUSED_SPACE_OFFSET; }
inline BitMove get_move_type(BitMove move) { return (move & MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET; }

inline bool is_promotion(BitMove move) { return (move & IS_PROMOTE_MASK); }
inline bool is_check(BitMove move) { return (move & IS_CHECK_MASK); }
inline bool is_capture(BitMove move) { return (move & IS_CAPTURE_MASK); }
inline BitMove get_misc_move_type(BitMove move) { return (move & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET; }

// kinda hacky but cool
inline bool is_castle(BitMove move) { return (move & (move - 1)); }

// for all setters, wipe the target data with the invert mask and set the requested value
inline void set_origin_index(BitMove& move, int origin_index) {
    move = (move & ~ORIGIN_INDEX_MASK) | (origin_index << ORIGIN_INDEX_OFFSET);
}

inline void set_destination_index(BitMove& move, int destination_index) {
    move = (move & ~DESTINATION_INDEX_MASK) | (destination_index << DESTINATION_INDEX_OFFSET);
}

// possibly redundant info
inline void set_white_move(BitMove& move, bool is_white) {
    if (is_white) {
        move |= IS_WHITE_MOVE_MASK;
    }
    else {
        move &= ~IS_WHITE_MOVE_MASK;
    }
}

inline void set_moving_type(BitMove& move, PieceType moving_type) {
    move = (move & ~MOVING_TYPE_MASK) | (moving_type << MOVING_TYPE_OFFSET);
}

inline void set_captured_type(BitMove& move, PieceType captured_type) {
    move = (move & ~CAPTURED_TYPE_MASK) | (captured_type << CAPTURED_TYPE_OFFSET);
}

// should NOT be used directly
inline void set_unused_space(BitMove& move, BitMove unused_space) {
    move = (move & ~UNUSED_SPACE_MASK) | (unused_space << UNUSED_SPACE_OFFSET);
}

// should NOT be used directly
inline void set_move_type(BitMove& move, BitMove move_type) {
    move = (move & ~MOVE_TYPE_MASK) | (move_type << MOVE_TYPE_OFFSET);
}

inline void set_promote(BitMove& move, bool is_promote) {
    if (is_promote) {
        move |= IS_PROMOTE_MASK;
    }
    else {
        move &= ~IS_PROMOTE_MASK;
    }
}

inline void set_check(BitMove& move, bool is_check) {
    if (is_check) {
        move |= IS_CHECK_MASK;
    }
    else {
        move &= ~IS_CHECK_MASK;
    }
}

inline void set_capture(BitMove& move, bool is_capture) {
    if (is_capture) {
        move |= IS_CAPTURE_MASK;
    }
    else {
        move &= ~IS_CAPTURE_MASK;
    }
}

inline void set_misc_move_type(BitMove& move, MoveType misc_move_type) {
    move = (move & ~MISC_MOVE_TYPE_MASK) | (misc_move_type << MOVE_TYPE_OFFSET);
}
