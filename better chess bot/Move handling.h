#pragma once

#include "Move structure.h"
#include "Board structure.h"
#include "Board operations.h"
#include "Position structure.h"

SidedPosition make_move(SidedPosition& sided_position, BitMove bitmove);
SidedPosition undo_move(SidedPosition& sided_position, BitMove bitmove);

void toggle_castle(SidedPosition& sided_position, const BitMove move);
void toggle_move(SidedPosition& sided_position, const BitMove);
void toggle_captured(SidedPosition& sided_position, const BitMove);
void toggle_promotion(SidedPosition& sided_position, const BitMove);
void set_special_move_rights(SidedPosition& sided_position, const BitMove move);