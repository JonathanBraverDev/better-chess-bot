#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Attack detection.h"
#include "Position structure.h"
#include "Position generation.h"

constexpr int DRAW_MOVE_LIMIT = 50;
constexpr int DRAW_REPETITIONS = 3;

// trying to avoind floats during computation (but also avoid overflow)
constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 310; // so the bot dosent want to go rook + pawn for 2 knights/bishops
constexpr int BISHOP_VALUE = 320; // slightly over a knight
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;
constexpr int WIN_VALUE = 100000; // aka 'stupidly large number' that you can't achive even with 64 queens
constexpr int DRAW_VALUE = 0; // possibly set negative to discourage draws

// simple eval
bool is_draw(GameState& current_state);
bool is_checkmate(SidedPosition sided_position);
int material_eval(SidedPosition sided_position);