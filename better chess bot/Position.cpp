#include "Position.h"
#include "Exceptions.h"

void Position::makeMove(Move move) {
	// should be mostly a copy from old code
}

std::vector<Move> Position::getPotentialMoves() const {
    std::vector<Move> moves;
    return moves;
}

void Position::getSlidingPieceMoves(std::vector<Move>& moves, const PieceType pieceType) const {
    Bitboard pieces = getPieces(current_color, pieceType);
    Bitboard ownPieces = getAllOwnPieces();
    Bitboard opponentPieces = getAllOpponentPieces();
    Bitboard allPieces = BitboardOperations::combineBoards(ownPieces, opponentPieces);
    Bitboard destinations;
    Bitboard captures;
    Move moveBase;

    Bitboard piece = pieces.popLowestBit(); // focus on the next piece

    while (piece.hasRemainingBits()) {
        moveBase.clear();
        destinations.clear();

        moveBase.setMoverColor(current_color);
        moveBase.setMovingOrPromotedType(pieceType);
        moveBase.setOriginIndex(piece.singleBitIndex());

        // Update destinations based on piece type
        switch (pieceType) {
        case PieceType::BISHOP:
            destinations = BitboardOperations::combineBoards(
                piece.slideUpLeft(allPieces),
                piece.slideUpRight(allPieces),
                piece.slideDownLeft(allPieces),
                piece.slideDownRight(allPieces)
            );
            break;
        case PieceType::ROOK:
            destinations = BitboardOperations::combineBoards(
                piece.slideUp(allPieces),
                piece.slideDown(allPieces),
                piece.slideLeft(allPieces),
                piece.slideRight(allPieces)
            );
            break;
        case PieceType::QUEEN:
            destinations = BitboardOperations::combineBoards(
                piece.slideUp(allPieces),
                piece.slideDown(allPieces),
                piece.slideLeft(allPieces),
                piece.slideRight(allPieces),
                piece.slideUpLeft(allPieces),
                piece.slideUpRight(allPieces),
                piece.slideDownLeft(allPieces),
                piece.slideDownRight(allPieces)
            );
            break;
        }

        destinations.clearBitsFrom(ownPieces);

        captures = BitboardOperations::findCommonBits(destinations, opponentPieces);
        destinations.clearBitsFrom(captures);

        addDestinationMoves(moves, moveBase, destinations);
        addCaptureMoves(moves, moveBase, captures);

        piece = pieces.popLowestBit();
    }
}

void Position::getBishopMoves(std::vector<Move>& moves) const {
    getSlidingPieceMoves(moves, PieceType::BISHOP);
}

void Position::getRookMoves(std::vector<Move>& moves) const {
    getSlidingPieceMoves(moves, PieceType::ROOK);
}

void Position::getQueenMoves(std::vector<Move>& moves) const {
    getSlidingPieceMoves(moves, PieceType::QUEEN);
}

void Position::addDestinationMoves(std::vector<Move>& moves, Move baseMove, Bitboard destinations) const {
    Bitboard destination = destinations.popLowestBit();
    Move move;

    while (destination.hasRemainingBits()) {
        move = baseMove; // reset the move

        move.setDestinationIndex(destination.singleBitIndex());
        moves.push_back(move);

        destination = destinations.popLowestBit();
    }
}

void Position::addCaptureMoves(std::vector<Move>& moves, Move baseMove, Bitboard captures) const {
    Bitboard capture = captures.popLowestBit();
    Move move;
    int destinationIndex = capture.singleBitIndex();

    while (capture.hasRemainingBits()) {
        move = baseMove; // reset the move

        move.setDestinationIndex(destinationIndex);
        move.setCapturedType(getPieceAtIndex(destinationIndex).type);
        move.setCapture(true); // very similar functions to "DestinationMoves" but saves the check on every tile

        moves.push_back(move);

        capture = captures.popLowestBit();
    }
}

Color Position::getOpponentColor() const {
    return (current_color == Color::WHITE ? Color::BLACK : Color::WHITE);
}

Bitboard Position::getPieces(Color color, PieceType type) const {
    switch (color) {
        case Color::WHITE:
            switch (type) {
                case PieceType::PAWN: return white_pawns;
                case PieceType::KNIGHT: return white_knights;
                case PieceType::BISHOP: return white_bishops;
                case PieceType::ROOK: return white_rooks;
                case PieceType::QUEEN: return white_queens;
                case PieceType::KING: return white_king;
                default: throw InvalidPieceTypeException();
            }
        case Color::BLACK:
            switch (type) {
                case PieceType::PAWN: return black_pawns;
                case PieceType::KNIGHT: return black_knights;
                case PieceType::BISHOP: return black_bishops;
                case PieceType::ROOK: return black_rooks;
                case PieceType::QUEEN: return black_queens;
                case PieceType::KING: return black_king;
                default: throw InvalidPieceTypeException();
            }
        default: throw InvalidColorException();
    }
}

Piece Position::getPieceAtIndex(int index) const {
    if (white_pawns.getBoard() & (1ULL << index)) {
        return { Color::WHITE, PieceType::PAWN };
    } else if (black_pawns.getBoard() & (1ULL << index)) {
        return { Color::BLACK, PieceType::PAWN };
    } else if (white_knights.getBoard() & (1ULL << index)) {
        return { Color::WHITE, PieceType::KNIGHT };
    } else if (black_knights.getBoard() & (1ULL << index)) {
        return { Color::BLACK, PieceType::KNIGHT };
    } else if (white_bishops.getBoard() & (1ULL << index)) {
        return { Color::WHITE, PieceType::BISHOP };
    } else if (black_bishops.getBoard() & (1ULL << index)) {
        return { Color::BLACK, PieceType::BISHOP };
    } else if (white_rooks.getBoard() & (1ULL << index)) {
        return { Color::WHITE, PieceType::ROOK };
    } else if (black_rooks.getBoard() & (1ULL << index)) {
        return { Color::BLACK, PieceType::ROOK };
    } else if (white_queens.getBoard() & (1ULL << index)) {
        return { Color::WHITE, PieceType::QUEEN };
    } else if (black_queens.getBoard() & (1ULL << index)) {
        return { Color::BLACK, PieceType::QUEEN };
    } else if (white_king.getBoard() & (1ULL << index)) {
        return { Color::WHITE, PieceType::KING };
    } else if (black_king.getBoard() & (1ULL << index)) {
        return { Color::BLACK, PieceType::KING };
    } else {
        return { Color::NONE, PieceType::NONE };
    }
}

bool Position::validate() const {
    const Bitboard white_pieces = BitboardOperations::combineBoards(white_pawns, white_knights, white_bishops, white_rooks,white_queens);

    const Bitboard black_pieces = BitboardOperations::combineBoards(black_pawns, black_knights, black_bishops, black_rooks, black_queens);

    // Check if there is only one king for each color
    if (white_king.countSetBits() != 1 || black_king.countSetBits() != 1) {
        return false;
    }

    // Check if there are no more than 15 pieces for each color (king already checked)
    if (white_pieces.countSetBits() > 15 || black_pieces.countSetBits() > 15) {
        return false;
    }

    // Check for overlaps between all piece types (overkill)
    if (BitboardOperations::findCommonBits(white_pawns, white_knights, white_bishops, white_rooks, white_queens,
                                           black_pawns, black_knights, black_bishops, black_rooks, black_queens).hasRemainingBits()) {
        return false;
    }

    return true;
}



Bitboard Position::getOwnPieces(PieceType type) const {
    return getPieces(current_color, type);
}

Bitboard Position::getOpponentPieces(PieceType type) const {
    return getPieces(getOpponentColor(), type);
}

Bitboard Position::getAllOwnPieces() const {
    return BitboardOperations::combineBoards(getOwnPieces(PieceType::PAWN),
        getOwnPieces(PieceType::KNIGHT),
        getOwnPieces(PieceType::BISHOP),
        getOwnPieces(PieceType::ROOK),
        getOwnPieces(PieceType::QUEEN),
        getOwnPieces(PieceType::KING));
}

Bitboard Position::getAllOpponentPieces() const {
    return BitboardOperations::combineBoards(getOpponentPieces(PieceType::PAWN),
        getOpponentPieces(PieceType::KNIGHT),
        getOpponentPieces(PieceType::BISHOP),
        getOpponentPieces(PieceType::ROOK),
        getOpponentPieces(PieceType::QUEEN),
        getOpponentPieces(PieceType::KING));
}