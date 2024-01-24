#include "GameState.h"
#include "Enums.h"

void GameState::makeMove(Move move) {
	// pass the call to the board

	// update color
	// update timer based on move
}

std::vector<Move> GameState::getLegalMoves() const {

	if (isCheck) {
		// evade moves only
	} else {
		// all possible moves
	}

	// verify no new check on moves that might uncover an attack (queen possible from king)

	return std::vector<Move>();
}
