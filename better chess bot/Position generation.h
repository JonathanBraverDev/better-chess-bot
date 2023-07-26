#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Game constants.h"
#include "Attack detection.h"
#include "Preallocation structure.h"

void visualize_position(const SidedPosition position);

bool is_castle_legal(const SidedPosition sided_position, const B64 king_start, const B64 king_end);

// all different position type generation
void possible_piece_positions(SearchPreallocation& allocation, const SidedPosition& sided_position, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*const move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const int index_scale = 1, const int first_index = 0);
void possible_pawn_moves(SearchPreallocation& allocation, const SidedPosition& sided_position, const B64 free_tiles, const B64 opponent_pieces);
void possible_capture_positions(SearchPreallocation& allocation, SidedPosition& sided_position, B64 potential_moves, B64& current_pieces);
void possible_pawn_promotions(std::vector<SidedPosition>& positions, SidedPosition sided_position);
void possible_castle_positions(std::vector<SidedPosition>& positions, SidedPosition sided_position);

void tile_capture_positions(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target, B64 killing_pieces);
void kills_to_tile(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board_bit);
void tile_move_positions(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board_bit, const PieceType piece_type, B64 possible_moves = NULL);
void moves_to_tiles(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board);

// final validation of generated boards
void all_possible_positions(SearchPreallocation& allocation, const SidedPosition sided_position);
void possible_evade_positions(SearchPreallocation& allocation, const SidedPosition sided_position);
void valid_positions(SearchPreallocation& allocation, std::vector<SidedPosition>& valid_positions, const SidedPosition sided_position);

inline void all_possible_positions(SearchPreallocation& allocation, const GameState state) { all_possible_positions(allocation, state.sided_position); };
inline void possible_evade_positions(SearchPreallocation& allocation, const GameState state) { possible_evade_positions(allocation, state.sided_position); };