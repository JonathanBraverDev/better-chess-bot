#pragma once

#include "Board structure.h"

static B64 king_moves[64];
static B64 knight_moves[64];
static B64 pawn_moves[64 * 2]; // the 2 colors move in opposite direction
static B64 pawn_attacks[64 * 2]; // further conditional split
// rooks always have 14 potential moves, 7 horizontal and 7 vertical
// bishops can have as little as 7 moves and up to 13 (corner and center respectivly)

B64 generate_bishop_moves(const B64 all_pieces, B64 piece);
B64 generate_rook_moves(const B64 all_pieces, B64 piece);
B64 generate_queen_moves(const B64 all_pieces, B64 piece);
B64 generate_white_pawn_jump(const B64 all_pieces, B64 piece);
B64 generate_black_pawn_jump(const B64 all_pieces, B64 piece);

void prepare_king_moves();
void prepare_knight_moves();
void prepare_white_pawn_moves();
void prepare_black_pawn_moves();
void prepare_pawn_moves();
