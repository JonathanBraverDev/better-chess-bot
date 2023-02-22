#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"

constexpr int DRAW_MOVE_LIMIT = 50;
constexpr int DRAW_REPETITIONS = 3;

// trying to avoind floats during computation (but also avoid overflow)
constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 310; // so the bot dosent want to go rook + pawn for 2 knights/bishops
constexpr int BISHOP_VALUE = 320; // slightly over a knight
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;
constexpr int WIN_VALUE = 100000; // aka 'stupidly large number' that you can't achive even with 64 queens
constexpr int DRAW_VALUE = 0;

// simple eval
bool is_draw(GameState& current_state);
bool is_check(BoardPosition position, PlayerColor attacker_color);
B64 attacking_pieces(const BoardPosition position, const B64 target_board, const PlayerColor attacker_color);
bool is_checkmate(BoardPosition position, PlayerColor attacker_color);
int material_eval(BoardPosition position);

// less simple eval
int count_attacks(const B64 attacking_pieces, const B64 target_board, B64(*move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const B64 blockers = 0, const int index_scale = 1, const int first_index = 0);
// sliding attacks alias for count_attacks
int count_sliding_attacks(B64(*move_generator)(B64, B64), const B64 attacking_pieces, const B64 blockers, const B64 target_board);
// jumping attacks alias for	count_attacks
int count_jumping_attacks(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale = 1, const int first_index = 0);
int count_white_attacks(BoardPosition position, B64 target_board);
int count_black_attacks(BoardPosition position, B64 target_board);