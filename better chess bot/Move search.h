#pragma once
#include <utility>
#include <vector>
#include "Board structure.h"
#include "Evaluation.h"
#include "Board generation.h"

inline PlayerColor determine_player(const GameState state) { return (state.turn % 2 == 0 ? WHITE : BLACK); };
bool is_piece_taken(const GameState state, const BoardPosition position);

int alphabeta(GameState state, int depth, int alpha, int beta);
