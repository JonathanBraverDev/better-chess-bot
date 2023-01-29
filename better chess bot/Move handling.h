#pragma once

#include "Board structure.h"

struct Move {
	B64 origin;
	B64 destination;

	Move(int o, int d) : origin(o), destination(d) {}
};

inline Move Invert_move(Move move) { return Move(move.destination, move.origin); }

BoardPosition Make_move(BoardPosition position, Move move);
BoardPosition Undo_move(BoardPosition position, Move move);