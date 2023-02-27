#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Game constants.h"
#include "Attack detection.h"

bool is_castle_legal(const BoardPosition position, const bool is_king_white, const B64 king_start, const B64 king_end);

// all different board type generation
void possible_piece_positions(std::vector<BoardPosition>& positions, const BoardPosition position, const bool is_white, const B64 pieces, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const int index_scale = 1, const int first_index = 0);
void possible_pawn_move_positions(std::vector<BoardPosition>& positions, BoardPosition& new_position, const BoardPosition position, const bool is_white, const B64 piece, const B64 blockers, B64 potential_moves, B64& current_pieces);
void possible_capture_positions(std::vector<BoardPosition>& positions, std::vector<B64>& single_moves, BoardPosition& new_position, const BoardPosition position, const bool is_white, const B64 piece, B64& current_pieces, const B64& enemy_pawns, const B64& enemy_knights, const B64& enemy_bishops, const B64& enemy_rooks, const B64& enemy_queens, const B64* move_source = nullptr);
void possible_pawn_promotions(std::vector<BoardPosition>& positions, BoardPosition position, const bool is_white);
void possible_castle_positions(std::vector<BoardPosition>& positions, BoardPosition& position, const bool is_white, const B64 king);

// final validation of generated boards
std::vector<BoardPosition> all_possible_positions(const BoardPosition position, const bool is_white);
std::vector<BoardPosition> possible_evade_positions(const BoardPosition position, const bool is_white);
std::vector<BoardPosition> valid_positions(const GameState state);

inline std::vector<BoardPosition> all_possible_positions(const GameState state) { return all_possible_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)); };
inline std::vector<BoardPosition> possible_evade_positions(const GameState state) { return possible_evade_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)); };