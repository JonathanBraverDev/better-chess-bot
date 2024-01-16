#pragma once

#include <vector>
#include "Enums.h"
#include "Move.h"
#include "Position.h"

// game outcome and history
class Replay {
private:
	Color winnerColor;
	GameOverCause endType;
	std::vector<Move> moveHistory;
	std::vector<Position> stateHistory;

public:
	// TBD
};
