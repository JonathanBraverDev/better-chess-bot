#pragma once

#include <vector>
#include "Board structure.h"
#include "Evaluation.h"
#include "Board generation.h"
#include "Position structure.h"
#include "Position generation.h"

GameState generate_next_state(GameState state, const SidedPosition new_position);
inline int score_by_player(const bool is_white, int score) { return (is_white ? score : -score);  }
bool was_piece_taken(const SidedPosition original_position, const SidedPosition end_turn_position);

int alphabeta(GameState state, int depth, int alpha = -2 * WIN_VALUE, int beta = 2 * WIN_VALUE);
