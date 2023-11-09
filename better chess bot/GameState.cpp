#include "GameState.h"
#include "Enums.h"

std::vector<Move> GameState::getPotentialMoves() const {
	std::vector<Move> moves;
	return moves;
}

void GameState::getBishopMoves(std::vector<Move>& moves) const {
	Bitboard bishops = board.getPieces(current_color, PieceType::BISHOP);
	Bitboard ownPieces = getAllOwnPieces();
	Bitboard opponentPieces = getAllOpponentPieces();
	Bitboard allPieces = ownPieces.getBoard() | opponentPieces.getBoard();
	Bitboard destinations;
	Bitboard captures;
	Move moveBase;
	
	Bitboard bishop = bishops.popLowestBit(); // focus on the next bishop

	while (bishop.getBoard()) { // check that a bishop exists
		moveBase = Move(0); // reset the move
		destinations = Bitboard(0);

		moveBase.setMoverColor(current_color);
		moveBase.setMovingOrPromotedType(PieceType::BISHOP);
		moveBase.setOriginIndex(bishop.singleBitIndex());

		destinations = (bishop.slideUpLeft(allPieces).getBoard() |
						bishop.slideUpRight(allPieces).getBoard() |
						bishop.slideDownLeft(allPieces).getBoard() |
						bishop.slideDownRight(allPieces).getBoard());
		destinations.clearBitsFrom(ownPieces);

		captures = destinations.getBoard() & opponentPieces.getBoard();
		destinations.clearBitsFrom(captures);
		
		addDestinationMoves(moves, moveBase, destinations);
		addCaptureMoves(moves, moveBase, captures);

		bishop = bishops.popLowestBit();
	}
}

void GameState::addDestinationMoves(std::vector<Move>& moves, Move baseMove, Bitboard destinations) const {
	Bitboard destination = destinations.popLowestBit();
	Move move;

	while (destination.getBoard()) {
		move = baseMove; // reset the move

		move.setDestinationIndex(destination.singleBitIndex());
		moves.push_back(move);

		destination = destinations.popLowestBit();
	}
}

void GameState::addCaptureMoves(std::vector<Move>& moves, Move baseMove, Bitboard captures) const {
	Bitboard capture = captures.popLowestBit();
	Move move;
	int destinationIndex = capture.singleBitIndex();

	while (capture.getBoard()) {
		move = baseMove; // reset the move

		move.setDestinationIndex(destinationIndex);
		move.setCapturedType(board.getPieceAtIndex(destinationIndex).type);
		move.setCapture(true);

		moves.push_back(move);

		capture = captures.popLowestBit();
	}
}

Color GameState::getOpponentColor() const {
	return (current_color == Color::WHITE ? Color::BLACK : Color::WHITE);
}

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
