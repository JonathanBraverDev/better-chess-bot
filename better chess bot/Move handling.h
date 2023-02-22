#pragma once

#include "Move structure.h"
#include "Board structure.h"
#include "Board operations.h"

inline bool Is_castle(Move move) { return move.type == CASTLE_SHORT || move.type == CASTLE_LONG; };
inline bool Is_capture(Move move) { return move.type == CAPTURE || move.type == PROMOTION_CAPTURE; };
inline bool Is_promotion(Move move) { return move.type == PROMOTION || move.type == PROMOTION_CAPTURE; };


BoardPosition Make_move(BoardPosition position, Move move);
BoardPosition Undo_move(BoardPosition position, Move move);

void Toggle_castle(BoardPosition& position, const Move& move);
void Move_piece(BoardPosition& position, const Move& move, bool reverse = false);
void Toggle_captured(BoardPosition& position, const Move& move);
void Toggle_promotion(BoardPosition& position, const Move& move);