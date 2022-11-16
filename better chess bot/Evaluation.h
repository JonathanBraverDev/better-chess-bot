#pragma once

#include "Board structure.h"

const int DRAW_MOVES = 50;
const int DRAW_REPETITIONS = 3;

const int PAWN_VALUE = 1;
const int KNIGHT_VALUE = 3;
const int BISHOP_VALUE = 3; // debatable, but its 'officially' worth 3 points.
const int ROOK_VALUE = 5;
const int QUEEN_VALUE = 9;


bool check_draw(GameState& current_state);
