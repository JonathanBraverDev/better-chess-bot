#pragma once

#include "Move structure.h"
#include "Board structure.h"
#include "Board operations.h"
#include "Position structure.h"

inline bool Is_promotion(BitMove move) { return move & IS_PROMOTE_MASK; }
inline bool Is_capture(BitMove move) { return move & IS_CAPTURE_MASK; }
inline bool Is_castle(BitMove move) { return (((move & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET) == CASTLE_SHORT ||
											  ((move & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET) == CASTLE_LONG); }


SidedPosition Make_move(SidedPosition& sided_position, BitMove bitmove);
SidedPosition Undo_move(SidedPosition& sided_position, BitMove bitmove);

void Toggle_castle(SidedPosition& sided_position, const Move& move);
void Move_piece(SidedPosition& sided_position, const Move& move);
void Toggle_captured(SidedPosition& sided_position, const Move& move);
void Toggle_promotion(SidedPosition& sided_position, const Move& move);
void set_special_move_rights(SidedPosition& sided_position, const Move& move);