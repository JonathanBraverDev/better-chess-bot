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
    getPawnMoves();
    getKnightMoves();
    getBishopMoves();
    getRookMoves();
    getQueenMoves();
    getKingMoves();
    return moves;
}

void Position::getPawnMoves() {
    Bitboard pawns = getPieces(current_color, PieceType::PAWN);
    Bitboard opponent_pieces = getAllOpponentPieces();
    Bitboard empty_tiles = BitboardOperations::combineBoards(getAllOwnPieces(), opponent_pieces).invertedCopy();
    Bitboard opponent_en_passant = (current_color == Color::WHITE ? BLACK_EN_PASSANT : WHITE_EN_PASSANT);
    Bitboard step;
    Bitboard captures;
    Bitboard special_move_board; // either a jump OR en-passant. never both.
    Move move_base = currentBitRights();
    Move special_move;
    Direction adjusted_direction_forward = (current_color == Color::WHITE ? Direction::UP : Direction::DOWN);
    int color_offset = (current_color == Color::WHITE ? 0 : 1);
    int pawn_index;
    int adjusted_pawn_row;

    Bitboard pawn = pawns.popLowestBit(); // focus on the next piece

    while (pawn.hasRemainingBits()) {
        move_base.clearMoveData();
        step.clear();
        captures.clear();
        special_move_board.clear();

        pawn_index = pawn.singleBitIndex();
        adjusted_pawn_row = (current_color == Color::WHITE ? pawn_index / BOARD_SIZE : BOARD_SIZE - (pawn_index / BOARD_SIZE));

        step = BitboardOperations::findCommonBits(precomputed_moves.pawn_moves[color_offset + 2 * pawn_index],
                                                  empty_tiles);
        captures = BitboardOperations::findCommonBits(precomputed_moves.pawn_attacks[color_offset + 2 * pawn_index],
                                                      opponent_pieces);

        move_base.setOriginIndex(pawn_index); // the only thing that can be set for sure

        switch (adjusted_pawn_row) {
        case PAWN_INITIAL_ROW:
            if (step.hasRemainingBits()) {
                // check jump if a step is possible
                special_move_board = BitboardOperations::findCommonBits(step.look(adjusted_direction_forward),
                                                          empty_tiles);

                if (special_move_board.hasRemainingBits()) {
                    // explicitly add jump move
                    special_move = move_base.copy();

                    special_move.setMovingType(PieceType::PAWN);
                    special_move.setMiscMoveType(MoveType::PAWN_UNIQE);
                    special_move.setDestinationIndex(special_move_board.singleBitIndex());

                    moves.push_back(special_move);
                }
            }
            addNormalPawnMoves(move_base, step, captures);

            break;
        case PAWN_ENPASSANT_ROW:
            // check if en-passant is possible
            special_move_board = BitboardOperations::findCommonBits(precomputed_moves.pawn_attacks[color_offset + 2 * pawn_index],
                                                              opponent_en_passant);
            if (special_move_board.hasRemainingBits()) {
                // explicitly add enpassant move
                special_move = move_base.copy();

                special_move.setAttackerType(AttackerType::PAWN);
                special_move.setMiscMoveType(MoveType::PAWN_UNIQE);
                special_move.setCapturedType(PieceType::PAWN);
                special_move.setDestinationIndex(special_move_board.singleBitIndex());
            }
            addNormalPawnMoves(move_base, step, captures);

            break;
        case PAWN_PRE_PROMOTION_ROW:
            if (step.hasRemainingBits()) {
                // add step promotions
                move_base.setDestinationIndex(step.singleBitIndex());

                for (PieceType promotionType : {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT}) {
                    move_base.setMovingType(promotionType);
                    moves.push_back(move_base);
                }
            }

            special_move_board = captures.popLowestBit();
            while (special_move_board.hasRemainingBits()) {
                // add capture promotions
                move_base.setDestinationIndex(special_move_board.singleBitIndex());
                move_base.setCapturedType(getPieceAtTile(special_move_board).type);
                move_base.setCapture(true); 

                // note that captures do NOT use attacker type to prioritize high promotions
                // this allows sorting by int value but complicates decode a bit
                for (PieceType promotionType : {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT}) {
                    move_base.setMovingType(promotionType);
                    moves.push_back(move_base);
                }

                special_move_board = captures.popLowestBit();
            }
            break;
        default:
            addNormalPawnMoves(move_base, step, captures);
            break;
        }

        pawn = pawns.popLowestBit();
    }
}

void Position::addNormalPawnMoves(Move move_base, Bitboard step, Bitboard captures) {
    // base_move is expected to have the origin set
    // note that the passed move is never safely reset

    Bitboard capture = captures.popLowestBit();

    if (step.hasRemainingBits()) {
        // add step
        move_base.setMovingType(PieceType::PAWN);
        move_base.setDestinationIndex(step.singleBitIndex());

        moves.push_back(move_base);
    }

    while (capture.hasRemainingBits()) {
        // add capture
        move_base.setAttackerType(AttackerType::PAWN);
        move_base.setDestinationIndex(capture.singleBitIndex());
        move_base.setCapturedType(getPieceAtTile(capture).type);

        moves.push_back(move_base);
        capture = captures.popLowestBit();
    }
}

void Position::getKnightMoves() {
    Bitboard knights = getPieces(current_color, PieceType::KNIGHT);
    Bitboard own_pieces = getAllOwnPieces();
    Bitboard opponent_pieces = getAllOpponentPieces();
    Bitboard destinations;
    Move move_base;

    Bitboard knight = knights.popLowestBit(); // focus on the next piece

    while (knight.hasRemainingBits()) {
        move_base.clearMoveData();
        destinations.clear();

        move_base.setMovingType(PieceType::KNIGHT);
        move_base.setOriginIndex(knight.singleBitIndex());

        destinations = precomputed_moves.knight_moves[knight.singleBitIndex()];
        destinations.clearBitsFrom(own_pieces);

        finalizeMoves(destinations, own_pieces, opponent_pieces, move_base);

        knight = knights.popLowestBit();
    }
}
        
void Position::getSlidingPieceMoves(const PieceType pieceType) {
    Bitboard pieces = getPieces(current_color, pieceType);
    // this part repeats, shoud it be split off and passed around?
    Bitboard own_pieces = getAllOwnPieces();
    Bitboard opponent_pieces = getAllOpponentPieces();
    Bitboard all_pieces = BitboardOperations::combineBoards(own_pieces, opponent_pieces);
    Bitboard destinations;
    Move move_base;

    Bitboard piece = pieces.popLowestBit(); // focus on the next piece

    while (piece.hasRemainingBits()) {
        move_base.clearMoveData();
        destinations.clear();

        move_base.setMovingType(pieceType);
        move_base.setOriginIndex(piece.singleBitIndex());

        destinations = getSlideDestinations(piece, pieceType, all_pieces);
        destinations.clearBitsFrom(own_pieces);

        finalizeMoves(destinations, own_pieces, opponent_pieces, move_base);

        piece = pieces.popLowestBit();
    }
}

Bitboard Position::getSlideDestinations(const Bitboard piece, const PieceType pieceType, const Bitboard blockers) const {
    Bitboard destinations;

        switch (pieceType) {
        case PieceType::BISHOP:
            destinations = BitboardOperations::combineBoards(
            piece.slidePath(Direction::UP_LEFT, blockers),
            piece.slidePath(Direction::UP_RIGHT, blockers),
            piece.slidePath(Direction::DOWN_LEFT, blockers),
            piece.slidePath(Direction::DOWN_RIGHT, blockers)
            );
            break;
        case PieceType::ROOK:
            destinations = BitboardOperations::combineBoards(
            piece.slidePath(Direction::UP, blockers),
            piece.slidePath(Direction::DOWN, blockers),
            piece.slidePath(Direction::LEFT, blockers),
            piece.slidePath(Direction::RIGHT, blockers)
            );
            break;
        case PieceType::QUEEN:
            destinations = BitboardOperations::combineBoards(
            piece.slidePath(Direction::UP, blockers),
            piece.slidePath(Direction::DOWN, blockers),
            piece.slidePath(Direction::LEFT, blockers),
            piece.slidePath(Direction::RIGHT, blockers),
            piece.slidePath(Direction::UP_LEFT, blockers),
            piece.slidePath(Direction::UP_RIGHT, blockers),
            piece.slidePath(Direction::DOWN_LEFT, blockers),
            piece.slidePath(Direction::DOWN_RIGHT, blockers)
            );
            break;
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

void Position::getKingMoves() {
    Bitboard king = getPieces(current_color, PieceType::KNIGHT);
    Bitboard own_pieces = getAllOwnPieces();
    Bitboard opponent_pieces = getAllOpponentPieces();
    Bitboard destinations;
    Move move_base;

    move_base.setMovingType(PieceType::KNIGHT);
    move_base.setOriginIndex(king.singleBitIndex());

    destinations = precomputed_moves.king_moves[king.singleBitIndex()];
    destinations.clearBitsFrom(own_pieces);

    finalizeMoves(destinations, own_pieces, opponent_pieces, move_base);

    // add castles
}

// filters out 'frienly fire' and saves the moves from the destination board
void Position::finalizeMoves(Bitboard destinations, Bitboard own_pieces, Bitboard opponent_pieces, Move move_base) {
    destinations.clearBitsFrom(own_pieces);

    Bitboard captures = BitboardOperations::findCommonBits(destinations, opponent_pieces);
    destinations.clearBitsFrom(captures);

    addDestinationMoves(move_base, destinations);
    move_base.setAttackerType(pieceTypeToAttackerMap.at(move_base.getMovingOrPromotedType()));
    addCaptureMoves(move_base, captures);
}

void Position::addDestinationMoves(Move move_base, Bitboard destinations) {
    Bitboard destination = destinations.popLowestBit();
    Move move;

    while (destination.hasRemainingBits()) {
        move = move_base; // reset the move

        move.setDestinationIndex(destination.singleBitIndex());
        moves.push_back(move);

        destination = destinations.popLowestBit();
    }
}

void Position::addCaptureMoves(Move move_base, Bitboard captures) {
    Bitboard capture = captures.popLowestBit();
    Move move;

    while (capture.hasRemainingBits()) {
        move = move_base; // reset the move

        move.setDestinationIndex(capture.singleBitIndex());
        move.setCapturedType(getPieceAtTile(capture).type);
        move.setCapture(true); // very similar functions to "DestinationMoves" but saves the check on every tile

        moves.push_back(move);

        capture = captures.popLowestBit();
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
    Bitboard tile = (1ULL << index);
    return getPieceAtTile(tile);
}

Piece Position::getPieceAtTile(Bitboard tile) const {
    if (BitboardOperations::findCommonBits(white_pawns, tile).hasRemainingBits()) {
        return { Color::WHITE, PieceType::PAWN };
    } else if (BitboardOperations::findCommonBits(black_pawns, tile).hasRemainingBits()) {
        return { Color::BLACK, PieceType::PAWN };
    } else if (BitboardOperations::findCommonBits(white_knights, tile).hasRemainingBits()) {
        return { Color::WHITE, PieceType::KNIGHT };
    } else if (BitboardOperations::findCommonBits(black_knights, tile).hasRemainingBits()) {
        return { Color::BLACK, PieceType::KNIGHT };
    } else if (BitboardOperations::findCommonBits(white_bishops, tile).hasRemainingBits()) {
        return { Color::WHITE, PieceType::BISHOP };
    } else if (BitboardOperations::findCommonBits(black_bishops, tile).hasRemainingBits()) {
        return { Color::BLACK, PieceType::BISHOP };
    } else if (BitboardOperations::findCommonBits(white_rooks, tile).hasRemainingBits()) {
        return { Color::WHITE, PieceType::ROOK };
    } else if (BitboardOperations::findCommonBits(black_rooks, tile).hasRemainingBits()) {
        return { Color::BLACK, PieceType::ROOK };
    } else if (BitboardOperations::findCommonBits(white_queens, tile).hasRemainingBits()) {
        return { Color::WHITE, PieceType::QUEEN };
    } else if (BitboardOperations::findCommonBits(black_queens, tile).hasRemainingBits()) {
        return { Color::BLACK, PieceType::QUEEN };
    } else if (BitboardOperations::findCommonBits(white_king, tile).hasRemainingBits()) {
        return { Color::WHITE, PieceType::KING };
    } else if (BitboardOperations::findCommonBits(black_king, tile).hasRemainingBits()) {
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

BitMove Position::currentBitRights() {
    // check if white king has castle rights
    // find both white rooks, check if they have castle rights
    // check if black king has rights
    // check if black roosk have rights
    // check correct en passant row to ne non 0
    // check index of active en passant bit if any
    return BitMove();
}
