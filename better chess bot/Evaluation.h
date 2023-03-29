#pragma once

#include "Board structure.h"
#include "Board operations.h"
#include "Board generation.h"
#include "Attack detection.h"
#include "Position structure.h"
#include "Position generation.h"
#include "Evaluation structure.h"

constexpr int DRAW_MOVE_LIMIT = 50;
constexpr int DRAW_REPETITIONS = 3;

// simple eval
bool is_draw(GameState& current_state);
bool is_checkmate(SidedPosition sided_position);
int material_eval(SidedPosition sided_position);