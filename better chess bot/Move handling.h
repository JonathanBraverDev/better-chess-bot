#pragma once

#include "Move structure.h"
#include "Board structure.h"
#include "Board operations.h"
#include "Position structure.h"

inline bool Is_castle(Move move) { return move.type == CASTLE_SHORT || move.type == CASTLE_LONG; };
inline bool Is_capture(Move move) { return move.type == CAPTURE || move.type == PROMOTION_CAPTURE; };
inline bool Is_promotion(Move move) { return move.type == PROMOTION || move.type == PROMOTION_CAPTURE; };


SidedPosition Make_move(SidedPosition sided_position, Move move);
SidedPosition Undo_move(SidedPosition sided_position, Move move);

void Toggle_castle(SidedPosition& sided_position, const Move& move);
void Move_piece(SidedPosition& sided_position, const Move& move);
void Toggle_captured(SidedPosition& sided_position, const Move& move);
void Toggle_promotion(SidedPosition& sided_position, const Move& move);
void set_special_move_rights(SidedPosition& sided_position, const Move& move);