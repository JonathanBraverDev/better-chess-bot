#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"

// for validating moves
bool is_check(const BoardPosition position, const PlayerColor attacker_color);
inline bool is_check(const GameState state) { return is_check(state.position, determine_player(state)); };
B64 attacking_pieces(const BoardPosition position, const B64 target_board, const PlayerColor attacker_color);

// all different board type generation
void possible_piece_positions(std::vector<BoardPosition>& positions, const BoardPosition position, const PlayerColor color, const B64 pieces, const PieceType piece_type, const B64 blockers, const B64 valid_destinations, B64(*move_generator)(B64, B64) = nullptr, const B64* move_source = nullptr, const int index_scale = 1, const int first_index = 0);
void possible_pawn_move_positions(std::vector<BoardPosition>& positions, BoardPosition& new_position, const BoardPosition position, const PlayerColor color, const B64 piece, const B64 blockers, B64 potential_moves, B64& current_pieces);
void possible_capture_positions(std::vector<BoardPosition>& positions, std::vector<B64>& single_moves, BoardPosition& new_position, const BoardPosition position, const PlayerColor color, const B64 piece, B64& current_pieces, const B64& enemy_pawns, const B64& enemy_knights, const B64& enemy_bishops, const B64& enemy_rooks, const B64& enemy_queens, const B64* move_source = nullptr);
void possible_pawn_promotions(std::vector<BoardPosition>& positions, BoardPosition position, const PlayerColor color);

// final validation of generated boards
std::vector<BoardPosition> all_possible_positions(const BoardPosition position, const PlayerColor color);
std::vector<BoardPosition> possible_evade_positions(const BoardPosition position, const PlayerColor color);
std::vector<BoardPosition> valid_positions(const GameState state);

inline std::vector<BoardPosition> all_possible_positions(const GameState state) { return all_possible_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)); };
inline std::vector<BoardPosition> possible_evade_positions(const GameState state) { return possible_evade_positions(state.position, (state.turn % 2 == 0 ? WHITE : BLACK)); };