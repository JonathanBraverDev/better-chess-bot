#pragma once

#include "Board structure.h"

const int DRAW_MOVES = 50;
const int DRAW_REPETITIONS = 3;

// trying to avoind floats during computation (but also avoid overflow)
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 310; // so the bot dosent want to go rook + pawn for 2 knights/bishops
const int BISHOP_VALUE = 320; // slightly over a knight
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int WIN_VALUE = 100000; // aka 'stupidly large number' that you can't achive even with 64 queens
const int DRAW_VALUE = 0;

bool is_draw(GameState& current_state);
int material_eval(BoardPosition position);
