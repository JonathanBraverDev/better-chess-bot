#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Sided structure.h"

// boolean attacked
bool is_check(const BoardPosition position, const bool attacker_color);
bool is_check(const SidedPosition position);
inline bool is_check(const GameState state) { return is_check(state.position, is_white_player(state)); };
bool is_attacked(const B64 attacking_pieces, const B64 target_board, B64(*move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const B64 blockers = 0, const int index_scale = 1, const int first_index = 0);
bool is_slide_attacked(B64(*move_generator)(B64, B64), const B64 attacking_pieces, const B64 blockers, const B64 target_board);
bool is_jump_attacked(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale = 1, const int first_index = 0);
bool is_attackd_by_color(const SidedPosition sided_position, const B64 target_board);

// numeric attack detection
int count_attacks(const B64 attacking_pieces, const B64 target_board, B64(*move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const B64 blockers = 0, const int index_scale = 1, const int first_index = 0);
int count_sliding_attacks(B64(*move_generator)(B64, B64), const B64 attacking_pieces, const B64 blockers, const B64 target_board);
int count_jumping_attacks(const B64* move_source, const B64 attacking_pieces, const B64 target_board, const int index_scale = 1, const int first_index = 0);
int count_white_attacks(BoardPosition position, B64 target_board);
int count_black_attacks(BoardPosition position, B64 target_board);

// attacker detection
B64 attacking_pieces(const BoardPosition position, const B64 target_board, const bool attacker_color);