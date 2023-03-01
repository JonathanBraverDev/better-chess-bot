#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Game constants.h"
#include "Attack detection.h"
#include "Sided structure.h"

bool is_castle_legal(const BoardPosition position, const bool is_king_white, const B64 king_start, const B64 king_end);

// all different board type generation
void possible_piece_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*const move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const int index_scale = 1, const int first_index = 0);
void possible_pawn_move_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, const B64 piece, const B64 blockers, B64 potential_moves, B64& current_pieces);
void possible_capture_positions(std::vector<SidedPosition>& positions, const SidedPosition& sided_position, B64 potential_moves, const B64 piece, B64& current_pieces, const B64* move_source = nullptr);
void possible_pawn_promotions(std::vector<SidedPosition>& positions, SidedPosition sided_position);
void possible_castle_positions(std::vector<SidedPosition>& positions, SidedPosition& sided_position);

void tile_capture_positions(std::vector<SidedPosition>& positions, const SidedPosition sided_position, const B64 target, B64& current_pieces);
void kills_to_tile(std::vector<SidedPosition>& positions, const BoardPosition position, const B64 target_board_bit, const bool is_white);

// final validation of generated boards
std::vector<SidedPosition> all_possible_positions(const BoardPosition position, const bool is_white);
std::vector<SidedPosition> possible_evade_positions(const BoardPosition position, const bool is_white);
std::vector<SidedPosition> valid_positions(const GameState state);

inline std::vector<SidedPosition> all_possible_positions(const GameState state) { return all_possible_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)); };
inline std::vector<SidedPosition> possible_evade_positions(const GameState state) { return possible_evade_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)); };