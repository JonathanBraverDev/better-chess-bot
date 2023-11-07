#include "Board.h"
#include "Exceptions.h"

void Board::makeMove(Move move) {
	// should be mostly a copy from old code
}

Bitboard Board::getPieces(Color color, PieceType type) const {
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

Piece Board::getPieceAtIndex(int index) const {
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

bool Board::validate() const {
    const Bitboard white_pieces = white_pawns.getBoard() | white_knights.getBoard() | white_bishops.getBoard() |
        white_rooks.getBoard() | white_queens.getBoard();

    const Bitboard black_pieces = black_pawns.getBoard() | black_knights.getBoard() | black_bishops.getBoard() |
        black_rooks.getBoard() | black_queens.getBoard();

    const Bitboard* boards[] = {
        &white_pawns, &white_knights, &white_bishops,
        &white_rooks, &white_queens, &white_king,
        &black_pawns, &black_knights, &black_bishops,
        &black_rooks, &black_queens, &black_king
    };

    int boardNum = sizeof(boards) / sizeof(boards[0]);

    // Check if there is only one king for each color
    if (white_king.countSetBits() != 1 || black_king.countSetBits() != 1) {
        return false;
    }

    // Check if there are no more than 15 pieces for each color (king already checked)
    if (white_pieces.countSetBits() > 15 || black_pieces.countSetBits() > 15) {
        return false;
    }

    // Check for overlaps between all piece types (overkill)
    for (int i = 0; i < boardNum; ++i) {
        for (int j = i + 1; j < boardNum; ++j) {
            if (boards[i]->getBoard() & boards[j]->getBoard()) {
                return false;
            }
        }
    }

    return true;
}
