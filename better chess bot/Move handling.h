#pragma once

#include "Board structure.h"

struct Move {
	B64 origin;
	B64 destination;
};

Move Invert_move(Move move);

BoardPosition Make_move(BoardPosition position, Move move);
BoardPosition Undo_move(BoardPosition position, Move move);