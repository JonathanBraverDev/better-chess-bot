#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Game constants.h"
#include "Attack detection.h"
#include "Preallocation structure.h"

void visualize_position(const SidedPosition position);

bool is_castle_legal(const SidedPosition sided_position, const B64 king_start, const B64 king_end);
PieceType check_capture_type(const SidedPosition& sided_position, B64 target);

// all different position type generation
void possible_piece_moves(SearchPreallocation& allocation, const SidedPosition& sided_position, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*const move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const int index_scale = 1, const int first_index = 0);
void possible_pawn_moves(SearchPreallocation& allocation, const SidedPosition& sided_position, const B64 free_tiles, const B64 opponent_pieces);
void possible_capture_moves(SearchPreallocation& allocation, SidedPosition& sided_position, B64 piece, B64 potential_moves);
void possible_pawn_promotions(std::vector<BitMove>& moves, BitMove base_move);
void possible_castle_moves(std::vector<BitMove>& moves, SidedPosition sided_position);

void tile_capture_positions(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target, B64 killing_pieces, PieceType piece_type);
void kills_to_tile(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board_bit);
void tile_move_positions(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board_bit, const PieceType piece_type, B64 possible_moves = NULL);
void moves_to_tiles(SearchPreallocation& allocation, const SidedPosition sided_position, const B64 target_board);

// final validation of generated boards
void all_possible_moves(SearchPreallocation& allocation, const SidedPosition sided_position);
void possible_evade_moves(SearchPreallocation& allocation, const SidedPosition sided_position);
void valid_moves(SearchPreallocation& allocation, std::vector<BitMove>& valid_moves, const SidedPosition sided_position);

inline void all_possible_moves(SearchPreallocation& allocation, const GameState state) { all_possible_moves(allocation, state.sided_position); };
inline void possible_evade_moves(SearchPreallocation& allocation, const GameState state) { possible_evade_moves(allocation, state.sided_position); };