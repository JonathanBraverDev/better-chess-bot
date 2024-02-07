#include "Position.h"
#include "Exceptions.h"
#include "structs.h"
#include <cassert>

// empty initizlization of static member
PrecomputedMoves Position::precomputed_moves;

void Position::makeMove(Move move) {
	// should be mostly a copy from old code
}

std::vector<Move> Position::getPotentialMoves() {
    return moves;
}

void Position::getSlidingPieceMoves(const PieceType pieceType) {
    Bitboard pieces = getPieces(current_color, pieceType);
    Bitboard ownPieces = getAllOwnPieces();
    Bitboard opponentPieces = getAllOpponentPieces();
    Bitboard allPieces = BitboardOperations::combineBoards(ownPieces, opponentPieces);
    Bitboard destinations;
    Bitboard captures;
    Move moveBase;

    Bitboard piece = pieces.popLowestBit(); // focus on the next piece

    while (piece.hasRemainingBits()) {
        moveBase.clear(); // todo: adapt to change in move strucutre, preserve BitRights
        destinations.clear();

        moveBase.setMovingType(pieceType);
        moveBase.setOriginIndex(piece.singleBitIndex());

        // Update destinations based on piece type
        switch (pieceType) {
        case PieceType::BISHOP:
            destinations = BitboardOperations::combineBoards(
                piece.slidePath(Direction::UP_LEFT, allPieces),
                piece.slidePath(Direction::UP_RIGHT, allPieces),
                piece.slidePath(Direction::DOWN_LEFT, allPieces),
                piece.slidePath(Direction::DOWN_RIGHT, allPieces)
            );
            break;
        case PieceType::ROOK:
            destinations = BitboardOperations::combineBoards(
                piece.slidePath(Direction::UP, allPieces),
                piece.slidePath(Direction::DOWN, allPieces),
                piece.slidePath(Direction::LEFT, allPieces),
                piece.slidePath(Direction::RIGHT, allPieces)
            );
            break;
        case PieceType::QUEEN:
            destinations = BitboardOperations::combineBoards(
                piece.slidePath(Direction::UP, allPieces),
                piece.slidePath(Direction::DOWN, allPieces),
                piece.slidePath(Direction::LEFT, allPieces),
                piece.slidePath(Direction::RIGHT, allPieces),
                piece.slidePath(Direction::UP_LEFT, allPieces),
                piece.slidePath(Direction::UP_RIGHT, allPieces),
                piece.slidePath(Direction::DOWN_LEFT, allPieces),
                piece.slidePath(Direction::DOWN_RIGHT, allPieces)
            );
            break;
        }

        destinations.clearBitsFrom(ownPieces);

        captures = BitboardOperations::findCommonBits(destinations, opponentPieces);
        destinations.clearBitsFrom(captures);

        addDestinationMoves(moveBase, destinations);
        addCaptureMoves(moveBase, captures);

        piece = pieces.popLowestBit();
    }
}

void Position::getBishopMoves() {
    getSlidingPieceMoves(PieceType::BISHOP);
}

void Position::getRookMoves() {
    getSlidingPieceMoves(PieceType::ROOK);
}

void Position::getQueenMoves() {
    getSlidingPieceMoves(PieceType::QUEEN);
}

void Position::addDestinationMoves(Move baseMove, Bitboard destinations) {
    Bitboard destination = destinations.popLowestBit();
    Move move;

    while (destination.hasRemainingBits()) {
        move = baseMove; // reset the move

        move.setDestinationIndex(destination.singleBitIndex());
        moves.push_back(move);

        destination = destinations.popLowestBit();
    }
}

void Position::addCaptureMoves(Move baseMove, Bitboard captures) {
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
        destinationIndex = capture.singleBitIndex();
    }
}

Color Position::getOpponentColor() const {
    return (current_color == Color::WHITE ? Color::BLACK : Color::WHITE);
}

Bitboard Position::getPieces(Color color, PieceType type) const {
    assert(color != Color::NONE && type != PieceType::NONE);
    switch (color) {
        case Color::WHITE:
            switch (type) {
                case PieceType::PAWN: return white_pawns;
                case PieceType::KNIGHT: return white_knights;
                case PieceType::BISHOP: return white_bishops;
                case PieceType::ROOK: return white_rooks;
                case PieceType::QUEEN: return white_queens;
                case PieceType::KING: return white_king;
            }
        case Color::BLACK:
            switch (type) {
                case PieceType::PAWN: return black_pawns;
                case PieceType::KNIGHT: return black_knights;
                case PieceType::BISHOP: return black_bishops;
                case PieceType::ROOK: return black_rooks;
                case PieceType::QUEEN: return black_queens;
                case PieceType::KING: return black_king;
            }
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

void Position::validate() const {
    const Bitboard white_pieces = BitboardOperations::combineBoards(white_pawns, white_knights, white_bishops, white_rooks,white_queens);

    const Bitboard black_pieces = BitboardOperations::combineBoards(black_pawns, black_knights, black_bishops, black_rooks, black_queens);

    // Check if there is only one king for each color
    if (white_king.countSetBits() != 1 || black_king.countSetBits() != 1) {
        throw MultipleKingsException();
    }

    // Check if there are no more than 15 pieces for each color (king already checked)
    if (white_pieces.countSetBits() > 15 || black_pieces.countSetBits() > 15) {
        throw InvalidPieceCountException();
    }

    // Check for overlaps between all piece types (overkill)
    if (BitboardOperations::findCommonBits(white_pawns, white_knights, white_bishops, white_rooks, white_queens,
                                           black_pawns, black_knights, black_bishops, black_rooks, black_queens).hasRemainingBits()) {
        throw OverlappingPiecesException();
    }

    // check that there are no 'floating' castle rights without a rook/king
    Bitboard castalbles = BitboardOperations::combineBoards(white_king, white_rooks, black_king, black_rooks);
    if ((special_move_rigths.getBoard() & (WHITE_CASTLE | BLACK_CASTLE)) != castalbles.getBoard()) {
        throw InvalidCastlingRights();
        // check that king is in the middle of rook rights?
    }

    // check that no more than one enpasant exists
    if (Bitboard(special_move_rigths.getBoard() & ALL_EN_PASSANT).countSetBits() <= 1) {
        throw MultipleEnpasants();
    }
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

void Position::PrepareKingMoves() {
    Bitboard king = Bitboard(1ULL);
    // It's not ideal that the board is completly detached from the loop...
    for (size_t i = 0; i < 64; i++) {
        precomputed_moves.king_moves[i] = BitboardOperations::combineBoards(king.look(Direction::UP),
                                                                            king.look(Direction::DOWN),
                                                                            king.look(Direction::LEFT),
                                                                            king.look(Direction::RIGHT),
                                                                            king.look(Direction::UP_LEFT),
                                                                            king.look(Direction::UP_RIGHT),
                                                                            king.look(Direction::DOWN_LEFT),
                                                                            king.look(Direction::DOWN_RIGHT));

        king.nextTile();
    }
}

void Position::PrepareKnightMoves() {
    Bitboard knight = Bitboard(1ULL);
    for (size_t i = 0; i < 64; i++) {
        precomputed_moves.knight_moves[i] = BitboardOperations::combineBoards(knight.look(Direction::KNIGHT_UP_LEFT),
                                                                              knight.look(Direction::KNIGHT_UP_RIGHT),
                                                                              knight.look(Direction::KNIGHT_DOWN_LEFT),
                                                                              knight.look(Direction::KNIGHT_DOWN_RIGHT),
                                                                              knight.look(Direction::KNIGHT_LEFT_UP),
                                                                              knight.look(Direction::KNIGHT_LEFT_DOWN),
                                                                              knight.look(Direction::KNIGHT_RIGHT_UP),
                                                                              knight.look(Direction::KNIGHT_RIGHT_DOWN));
        knight.nextTile();
    }
}

void Position::PrepareWhitePawnMoves() {
    Bitboard white_pawn = Bitboard(1ULL);
    for (size_t i = 0; i < 64 * 2; i += 2) {
        precomputed_moves.pawn_moves[i] = white_pawn.look(Direction::UP);

        precomputed_moves.pawn_attacks[i] = BitboardOperations::combineBoards(white_pawn.look(Direction::UP_LEFT),
                                                                              white_pawn.look(Direction::UP_RIGHT));

        white_pawn.nextTile();
    }
}

void Position::PrepareBlackPawnMoves() {
    Bitboard black_pawn = Bitboard(1ULL);
    for (size_t i = 1; i < 64 * 2; i += 2) {
        precomputed_moves.pawn_moves[i] = black_pawn.look(Direction::DOWN);

        precomputed_moves.pawn_attacks[i] = BitboardOperations::combineBoards(black_pawn.look(Direction::DOWN_LEFT),
                                                                              black_pawn.look(Direction::DOWN_RIGHT));

        black_pawn.nextTile();
    }
}

void Position::InitializeMoves() {
    PrepareKingMoves();
    PrepareKnightMoves();
    PrepareWhitePawnMoves();
    PrepareBlackPawnMoves();
}
