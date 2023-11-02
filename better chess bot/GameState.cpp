#include "GameState.h"
#include "Enums.h"

void GameState::makeMove(Move move) {
	// pass the call to the board
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

Bitboard GameState::getOwnPieces(Color color, PieceType type) const {
	return board.getPieces(color, type);
}

Bitboard GameState::getOpponentPieces(Color color, PieceType type) const {
	return board.getPieces((color == Color::WHITE ? Color::BLACK : Color::WHITE), type);
}
