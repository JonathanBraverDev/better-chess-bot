#pragma once

#include "Board structure.h"

const int DRAW_MOVES = 50;
const int DRAW_REPETITIONS = 3;

// trying to avoind floats during computation (but also avoid overflow)
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 310; // so the bot dosent want to go rook + pawn for 2 knights/bishops
const int BISHOP_VALUE = 320; // slightly over a knight
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int WIN_VALUE = 100000; // aka 'stupidly large number' that you can't achive even with 64 queens
const int DRAW_VALUE = 0;

// simple eval
bool is_draw(GameState& current_state);
int material_eval(BoardPosition position);

// less simple eval
int count_attacks(B64 attacking_pieces, B64 target_board, B64(*move_generator)(B64, B64) = nullptr, B64* move_source = nullptr, B64 blockers = 0, int index_scale = 1, int first_index = 0);
// sliding attacks alias for count_attacks
int count_sliding_attacks(B64(*move_generator)(B64, B64), B64 attacking_pieces, B64 blockers, B64 target_board);
// jumping attacks alias for	count_attacks
int count_jumping_attacks(B64* move_source, B64 attacking_pieces, B64 target_board, int index_scale = 1, int first_index = 0);
int count_white_attacks(BoardPosition position, B64 target_board);
int count_black_attacks(BoardPosition position, B64 target_board);