#pragma once

#include "Enums.h"
#include "Move.h"
#include "Position.h"
#include <vector>

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
