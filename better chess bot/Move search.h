#pragma once
#include <vector>
#include "Board structure.h"
#include "Evaluation.h"
#include "Board generation.h"
#include "Position generation.h"

inline int adjust_by_player(const PlayerColor color, int score) { return (color == WHITE ? score : -score);  }
bool is_piece_taken(const GameState state, const BoardPosition position);

int alphabeta(GameState state, int depth, int alpha, int beta);
