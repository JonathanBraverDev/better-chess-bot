#include "GameState.h"
#include "Enums.h"

std::vector<Move> GameState::getPotentialMoves() {
	std::vector<Move> moves;
	return moves;
}

void GameState::makeMove(Move move) {
	// pass the call to the board

	// update color
	// update timerbased on move
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

Bitboard GameState::getOwnPieces(PieceType type) const {
	return board.getPieces(current_color, type);
}

Bitboard GameState::getOpponentPieces(PieceType type) const {
	return board.getPieces(getOpponentColor(), type);
}

Bitboard GameState::getAllOwnPieces() const {
	return Bitboard(getOwnPieces(PieceType::PAWN).getBoard() |
				    getOwnPieces(PieceType::KNIGHT).getBoard() |
				    getOwnPieces(PieceType::BISHOP).getBoard() |
				    getOwnPieces(PieceType::ROOK).getBoard() |
				    getOwnPieces(PieceType::QUEEN).getBoard() |
				    getOwnPieces(PieceType::KING).getBoard());
}

Bitboard GameState::getAllOpponentPieces() const {
	return Bitboard(getOpponentPieces(PieceType::PAWN).getBoard() |
					getOpponentPieces(PieceType::KNIGHT).getBoard() |
					getOpponentPieces(PieceType::BISHOP).getBoard() |
					getOpponentPieces(PieceType::ROOK).getBoard() |
					getOpponentPieces(PieceType::QUEEN).getBoard() |
					getOpponentPieces(PieceType::KING).getBoard());
}
