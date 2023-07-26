#pragma once

#include <vector>
#include "Board structure.h"
#include "Evaluation.h"
#include "Board generation.h"
#include "Position structure.h"
#include "Position generation.h"
#include "Preallocation structure.h"
#include "Move handling.h"

extern unsigned long long cutoffs;
extern unsigned long long evals;
extern unsigned long long nodes;

struct PositionScore {
	SidedPosition next_position;
	int current_score;
};

GameState generate_next_state(GameState state, const BitMove move);
inline int score_by_player(const bool is_white, int score) { return (is_white ? score : -score);  }
int find_last_draw_reset(const GameState& current_state);
bool was_draw_reset(const SidedPosition original_position, const SidedPosition end_turn_position);

PositionScore alphabeta_init(GameState state, int depth);
int alphabeta(SearchPreallocation& allocation, GameState state, int depth, int own_best, int opponent_best);
