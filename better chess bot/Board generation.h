#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Move structure.h"

static B64 king_moves[64];
static B64 knight_moves[64];
static B64 pawn_moves[64 * 2]; // the 2 colors move in opposite direction
static B64 pawn_attacks[64 * 2]; // further conditional split

constexpr int MAX_ROOK_MOVES = 14;
constexpr int MAX_BISHOP_MOVES = 13;
constexpr int MAX_QUEEN_MOVES = MAX_ROOK_MOVES + MAX_BISHOP_MOVES;
constexpr int MAX_KNIGHT_MOVES = 8;
constexpr int MAX_PAWN_MOVES = 12; // YES, actually, kill both sides and forward, all being a promotion
constexpr int EXPECTED_PAWN_MOVES = 3; // a MUCH more reasonable number, even this is too much for the average pawn

// rooks always have 14 potential moves, 7 horizontal and 7 vertical
// bishops can have as little as 7 moves and up to 13 (corner and center respectivly)

// build precalculatable moves
void prepare_king_moves();
void prepare_knight_moves();
void prepare_white_pawn_moves();
void prepare_black_pawn_moves();
void prepare_pawn_moves();

// generage more complex moves
B64 generate_bishop_moves(const B64 all_pieces, B64 piece);
B64 generate_rook_moves(const B64 all_pieces, B64 piece);
B64 generate_queen_moves(const B64 all_pieces, B64 piece);
B64 generate_pawn_jump(const B64 blockers, B64 piece, B64(*direction)(B64));
inline B64 generate_pawn_jump(const B64 blockers, B64 piece, PlayerColor color) { return generate_pawn_jump(blockers, piece, (color == WHITE ? &up : &down)); };