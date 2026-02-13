#include "MoveGenerator.h"
#include "MoveTables.h"
#include "Enums.h"
#include <cassert>

std::vector<Move> MoveGenerator::getLegalMoves(const Position& pos) {
    std::vector<Move> moves;
    
    // Ensure MoveTables are initialized (safe to call multiple times if guarded, or rely on main)
    // MoveTables::initialize(); // Assuming main calls it or static initializer

    getPawnMoves(pos, moves);
    getKnightMoves(pos, moves);
    getBishopMoves(pos, moves);
    getRookMoves(pos, moves);
    getQueenMoves(pos, moves);
    getKingMoves(pos, moves);

    return moves;
}

void MoveGenerator::getPawnMoves(const Position& pos, std::vector<Move>& moves) {
    Color current_color = pos.getCurrentColor();
    Bitboard pawns = pos.getPieces(current_color, PieceType::PAWN);
    Bitboard empty_tiles = pos.getAllPieces().getInverted();
    Bitboard opponent_en_passant = pos.getOpponentEnPassant();

    Bitboard step;
    Bitboard captures;
    Move move_base = pos.currentBitRights();
    Direction direction_forward = (current_color == Color::WHITE ? Direction::UP : Direction::DOWN);
    int color_offset = (current_color == Color::WHITE ? 0 : 1);
    BoardIndex pawn_index;
    int pawn_move_index;
    int adjusted_pawn_row;

    Bitboard pawn = pawns.popLowestBit(); 

    while (pawn.hasRemainingBits()) {
        move_base.clearMoveData();
        step.clear();
        captures.clear();

        pawn_index = pawn.singleBitIndex();
        pawn_move_index = 2 * pawn_index + color_offset;
        adjusted_pawn_row = (current_color == Color::WHITE
            ? (pawn_index / BOARD_SIZE) + 1
            : BOARD_SIZE - (pawn_index / BOARD_SIZE));

        step = findCommonBits(MoveTables::pawn_moves[pawn_move_index], empty_tiles);
        captures = findCommonBits(MoveTables::pawn_attacks[pawn_move_index], pos.getAllOpponentPieces());

        move_base.setOriginIndex(pawn_index);
        switch (adjusted_pawn_row) {
        case PAWN_INITIAL_ROW:
            checkAndAddPawnJump(pos, moves, step, empty_tiles, move_base, direction_forward);
            addNormalPawnMoves(pos, moves, move_base, step, captures);
            break;
        case PAWN_ENPASSANT_ROW:
            checkAndAddEnPassant(pos, moves, opponent_en_passant, pawn_move_index, move_base);
            addNormalPawnMoves(pos, moves, move_base, step, captures);
            break;
        case PAWN_PRE_PROMOTION_ROW:
            addPromotionMoves(pos, moves, step, captures, move_base);
            break;
        default:
            addNormalPawnMoves(pos, moves, move_base, step, captures);
            break;
        }

        pawn = pawns.popLowestBit();
    }
}

void MoveGenerator::checkAndAddPawnJump(const Position& pos, std::vector<Move>& moves, Bitboard step, Bitboard empty_tiles, Move move_base, Direction forward) {
    if (step.hasRemainingBits()) {
        Bitboard jump = findCommonBits(step.look(forward), empty_tiles);

        if (jump.hasRemainingBits()) {
            move_base.setMovingType(PieceType::PAWN);
            move_base.setMiscMoveType(MoveType::PAWN_UNIQE);
            move_base.setDestinationIndex(jump.singleBitIndex());

            CheckAndSaveMove(pos, moves, move_base);
        }
    }
}

void MoveGenerator::checkAndAddEnPassant(const Position& pos, std::vector<Move>& moves, Bitboard potential_en_passant, int pawn_move_index, Move move_base) {
    Bitboard en_passant = findCommonBits(MoveTables::pawn_attacks[pawn_move_index],
        potential_en_passant, pos.getSpecialMoveRights());

    if (en_passant.hasRemainingBits()) {
        move_base.setAttackerType(AttackerType::PAWN);
        move_base.setMiscMoveType(MoveType::PAWN_UNIQE);
        move_base.setCapturedType(PieceType::PAWN);
        move_base.setDestinationIndex(en_passant.singleBitIndex());
        CheckAndSaveMove(pos, moves, move_base);
    }
}

void MoveGenerator::addPromotionMoves(const Position& pos, std::vector<Move>& moves, Bitboard step, Bitboard captures, Move move_base) {
    if (step.hasRemainingBits()) {
        move_base.setDestinationIndex(step.singleBitIndex());
        for (PieceType promotionType : {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT}) {
            move_base.setMovingType(promotionType);
            CheckAndSaveMove(pos, moves, move_base);
        }
    }

    Bitboard promotion = captures.popLowestBit();
    while (promotion.hasRemainingBits()) {
        move_base.setDestinationIndex(promotion.singleBitIndex());
        move_base.setCapturedType(pos.getPieceAtTile(promotion).type);
        move_base.setCapture(true);

        for (PieceType promotionType : {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT}) {
            move_base.setMovingType(promotionType);
            CheckAndSaveMove(pos, moves, move_base);
        }
        promotion = captures.popLowestBit();
    }
}

void MoveGenerator::addNormalPawnMoves(const Position& pos, std::vector<Move>& moves, Move move_base, Bitboard step, Bitboard captures) {
    Bitboard capture = captures.popLowestBit();

    if (step.hasRemainingBits()) {
        move_base.setMovingType(PieceType::PAWN);
        move_base.setDestinationIndex(step.singleBitIndex());
        CheckAndSaveMove(pos, moves, move_base);
    }

    while (capture.hasRemainingBits()) {
        move_base.setAttackerType(AttackerType::PAWN);
        move_base.setDestinationIndex(capture.singleBitIndex());
        move_base.setCapturedType(pos.getPieceAtTile(capture).type);

        CheckAndSaveMove(pos, moves, move_base);
        capture = captures.popLowestBit();
    }
}

void MoveGenerator::getKnightMoves(const Position& pos, std::vector<Move>& moves) {
    Bitboard knights = pos.getOwnPieces(PieceType::KNIGHT);
    Bitboard destinations;
    Move move_base;
    Bitboard knight = knights.popLowestBit();

    while (knight.hasRemainingBits()) {
        move_base.clearMoveData();
        destinations.clear();

        move_base.setMovingType(PieceType::KNIGHT);
        move_base.setOriginIndex(knight.singleBitIndex());

        destinations = MoveTables::knight_moves[knight.singleBitIndex()];
        destinations.clearBitsFrom(pos.getAllOwnPieces());

        finalizeMoves(pos, moves, destinations, move_base);

        knight = knights.popLowestBit();
    }
}

void MoveGenerator::getSlidingPieceMoves(const Position& pos, std::vector<Move>& moves, const PieceType pieceType) {
    Bitboard pieces = pos.getOwnPieces(pieceType);
    Bitboard destinations;
    Move move_base;
    Bitboard piece = pieces.popLowestBit();

    while (piece.hasRemainingBits()) {
        move_base.clearMoveData();
        destinations.clear();

        move_base.setMovingType(pieceType);
        move_base.setOriginIndex(piece.singleBitIndex());

        if (pieceType == PieceType::QUEEN) {
            destinations = combineBoards(
                getSlideDestinations(pos, piece, AttackPattern::LINE),
                getSlideDestinations(pos, piece, AttackPattern::DIAGONAL));
        } else if (pieceType == PieceType::ROOK) {
            destinations = getSlideDestinations(pos, piece, AttackPattern::LINE);
        } else if (pieceType == PieceType::BISHOP) {
            destinations = getSlideDestinations(pos, piece, AttackPattern::DIAGONAL);
        }

        destinations.clearBitsFrom(pos.getAllOwnPieces());
        finalizeMoves(pos, moves, destinations, move_base);
        piece = pieces.popLowestBit();
    }
}

Bitboard MoveGenerator::getSlideDestinations(const Position& pos, const Bitboard piece, const AttackPattern pattern) {
    return getSlideDestinations(pos, piece, pattern, pos.getAllPieces());
}

Bitboard MoveGenerator::getSlideDestinations(const Position& pos, const Bitboard piece, const AttackPattern pattern, const Bitboard blockers) {
    assert(pattern == AttackPattern::DIAGONAL || pattern == AttackPattern::LINE);
    Bitboard destinations;

    switch (pattern) {
    case AttackPattern::DIAGONAL:
        destinations = combineBoards(
            piece.slidePath(Direction::UP_LEFT, blockers),
            piece.slidePath(Direction::UP_RIGHT, blockers),
            piece.slidePath(Direction::DOWN_LEFT, blockers),
            piece.slidePath(Direction::DOWN_RIGHT, blockers));
        break;
    case AttackPattern::LINE:
        destinations = combineBoards(
            piece.slidePath(Direction::UP, blockers),
            piece.slidePath(Direction::DOWN, blockers),
            piece.slidePath(Direction::LEFT, blockers),
            piece.slidePath(Direction::RIGHT, blockers));
        break;
    }
    return destinations;
}

void MoveGenerator::getBishopMoves(const Position& pos, std::vector<Move>& moves) {
    getSlidingPieceMoves(pos, moves, PieceType::BISHOP);
}

void MoveGenerator::getRookMoves(const Position& pos, std::vector<Move>& moves) {
    getSlidingPieceMoves(pos, moves, PieceType::ROOK);
}

void MoveGenerator::getQueenMoves(const Position& pos, std::vector<Move>& moves) {
    getSlidingPieceMoves(pos, moves, PieceType::QUEEN);
}

void MoveGenerator::getKingMoves(const Position& pos, std::vector<Move>& moves) {
    Bitboard king = pos.getOwnPieces(PieceType::KING);
    Bitboard own_pieces = pos.getAllOwnPieces();
    Bitboard opponent_pieces = pos.getAllOpponentPieces();
    Bitboard destinations;
    Move move_base;

    move_base.setMovingType(PieceType::KING);
    move_base.setOriginIndex(king.singleBitIndex());

    destinations = MoveTables::king_moves[king.singleBitIndex()];
    destinations.clearBitsFrom(own_pieces);

    finalizeMoves(pos, moves, destinations, move_base);
    getCastlingMoves(pos, moves, king, combineBoards(own_pieces, opponent_pieces), move_base);
}

void MoveGenerator::getCastlingMoves(const Position& pos, std::vector<Move>& moves, Bitboard king, Bitboard blockers, Move move_base) {
    if (!findCommonBits(king, pos.getSpecialMoveRights()).hasRemainingBits() ||
        isAttackedByAnyPattern(pos, king, pos.getAllPieces())) {
        return;
    }

    Color current_color = pos.getCurrentColor();
    Bitboard own_rooks = pos.getOwnPieces(PieceType::ROOK);
    Bitboard own_castle_row = (current_color == Color::WHITE ? WHITE_CASTLE_ROW : BLACK_CASTLE_ROW);
    Bitboard long_rook = findCommonBits(king.lowerThanSingleBit(), own_rooks, pos.getSpecialMoveRights(), own_castle_row);
    Bitboard short_rook = findCommonBits(king.higherThanSingleBit(), own_rooks, pos.getSpecialMoveRights(), own_castle_row);

    Bitboard long_king_dest = findCommonBits(own_castle_row, Bitboard(LONG_CASTLE_KING_COLUMN));
    Bitboard long_rook_dest = findCommonBits(own_castle_row, Bitboard(LONG_CASTLE_ROOK_COLUMN));
    Bitboard short_king_dest = findCommonBits(own_castle_row, Bitboard(SHORT_CASTLE_KING_COLUMN));
    Bitboard short_rook_dest = findCommonBits(own_castle_row, Bitboard(SHORT_CASTLE_ROOK_COLUMN));

    move_base = pos.currentBitRights();
    move_base.setOriginIndex(king.singleBitIndex());
    move_base.setMovingType(PieceType::KING);

    if (long_rook.hasRemainingBits() && canCastleWithRook(pos, king, long_rook, long_king_dest, long_rook_dest)) {
        move_base.setDestinationIndex(long_rook.singleBitIndex());
        move_base.setMiscMoveType(MoveType::CASTLE_LONG);
        moves.push_back(move_base);
    }

    if (short_rook.hasRemainingBits() && canCastleWithRook(pos, king, short_rook, short_king_dest, short_rook_dest)) {
        move_base.setDestinationIndex(short_rook.singleBitIndex());
        move_base.setMiscMoveType(MoveType::CASTLE_SHORT);
        moves.push_back(move_base);
    }
}

bool MoveGenerator::canCastleWithRook(const Position& pos, const Bitboard king, const Bitboard rook, const Bitboard king_dest, const Bitboard rook_dest) {
    if (!rook.getCommonBitsWith(pos.getSpecialMoveRights()).hasRemainingBits()) {
        return false;
    }

    Bitboard all_pieces = pos.getAllPieces();
    Bitboard king_path = king.sameRowPathTo(king_dest);
    Bitboard rook_path = rook.sameRowPathTo(rook_dest);
    Bitboard castling_pieces = combineBoards(king, rook);
    Bitboard blockers = all_pieces.getWithoutBitsFrom(castling_pieces);

    if (isAttackedByAnyPattern(pos, king_path, blockers)) {
        return false;
    }

    Bitboard interfering_pieces = combineBoards(findCommonBits(blockers, rook_path), findCommonBits(blockers, king_path));
    if (interfering_pieces.hasRemainingBits()) {
        return false;
    }

    return true;
}

void MoveGenerator::finalizeMoves(const Position& pos, std::vector<Move>& moves, Bitboard destinations, Move move_base) {
    destinations.clearBitsFrom(pos.getAllOwnPieces());
    Bitboard captures = findCommonBits(destinations, pos.getAllOpponentPieces());
    destinations.clearBitsFrom(captures);
    captures.clearBitsFrom(pos.getPieces(pos.getOpponentColor(), PieceType::KING)); 

    addDestinationMoves(pos, moves, destinations, move_base);
    move_base.setAttackerType(pieceTypeToAttackerMap.at(move_base.getAbsoluteMovingType()));
    addCaptureMoves(pos, moves, captures, move_base);
}

void MoveGenerator::addDestinationMoves(const Position& pos, std::vector<Move>& moves, Bitboard destinations, Move move_base) {
    Bitboard destination = destinations.popLowestBit();
    Move move;
    while (destination.hasRemainingBits()) {
        move = move_base;
        move.setDestinationIndex(destination.singleBitIndex());
        CheckAndSaveMove(pos, moves, move);
        destination = destinations.popLowestBit();
    }
}

void MoveGenerator::addCaptureMoves(const Position& pos, std::vector<Move>& moves, Bitboard captures, Move move_base) {
    Bitboard capture = captures.popLowestBit();
    Move move;
    while (capture.hasRemainingBits()) {
        move = move_base;
        move.setDestinationIndex(capture.singleBitIndex());
        move.setCapturedType(pos.getPieceAtTile(capture).type);
        move.setCapture(true);
        CheckAndSaveMove(pos, moves, move);
        capture = captures.popLowestBit();
    }
}

void MoveGenerator::CheckAndSaveMove(const Position& pos, std::vector<Move>& moves, Move proposed_move) {
    if (!selfCheckCheck(pos, proposed_move)) {
        if (enemyCheckCheck(pos, proposed_move)) {
            proposed_move.setCheck(true);
        }
        moves.push_back(proposed_move);
    }
}

bool MoveGenerator::selfCheckCheck(const Position& pos, Move proposed_move) {
    Bitboard own_king = pos.getOwnPieces(PieceType::KING);
    BoardIndex king_index;
    Bitboard blockers = pos.getAllPieces();

    blockers.clearBit(proposed_move.getOriginIndex());
    blockers.setBit(proposed_move.getDestinationIndex());

    BoardIndex capture_excluded = INVALID_INDEX;
    if (proposed_move.isCapture()) {
        capture_excluded = proposed_move.getDestinationIndex();
    }

    if (proposed_move.getAbsoluteMovingType() == PieceType::KING) {
        own_king.clear();
        own_king.setBit(proposed_move.getDestinationIndex());
        king_index = proposed_move.getDestinationIndex();

        if (isAttackedBySlidePattern(pos, own_king, AttackPattern::LINE, blockers, capture_excluded) ||
            isAttackedBySlidePattern(pos, own_king, AttackPattern::DIAGONAL, blockers, capture_excluded)) {
            return true;
        }

        if (isAttackedByJumpPattern(pos, king_index, AttackPattern::KNIGHT, capture_excluded) ||
            isAttackedByJumpPattern(pos, king_index, AttackPattern::PAWN, capture_excluded) ||
            isAttackedByJumpPattern(pos, king_index, AttackPattern::KING, capture_excluded)) {
            return true;
        }
    } else {
        king_index = own_king.singleBitIndex();

        if (proposed_move.isEnPassant()) {
            blockers.clearBitsFrom(Position::getEnPassantCaptureLocation(pos.getCurrentColor(), proposed_move.getDestinationIndex()));
        }

        if (isAttackedBySlidePattern(pos, own_king, AttackPattern::LINE, blockers, capture_excluded) ||
            isAttackedBySlidePattern(pos, own_king, AttackPattern::DIAGONAL, blockers, capture_excluded)) {
            return true;
        }

        if (pos.isInCheck()) {
            BoardIndex jump_excluded = capture_excluded;
            if (proposed_move.isEnPassant()) {
                jump_excluded = Position::getEnPassantCaptureLocation(pos.getCurrentColor(), proposed_move.getDestinationIndex()).singleBitIndex();
            }

            if (isAttackedByJumpPattern(pos, king_index, AttackPattern::KNIGHT, jump_excluded) ||
                isAttackedByJumpPattern(pos, king_index, AttackPattern::PAWN, jump_excluded)) {
                return true;
            }
        }
    }
    return false;
}

bool MoveGenerator::isAttackedBySlidePattern(const Position& pos, Bitboard target, AttackPattern pattern, Bitboard blockers, BoardIndex excluded_index) {
    assert(pattern == AttackPattern::DIAGONAL || pattern == AttackPattern::LINE);
    Bitboard slide_path = getSlideDestinations(pos, target, pattern, blockers);
    Bitboard slide_attackers = pos.getPiecesByPattern(pos.getOpponentColor(), pattern);
    if (excluded_index != INVALID_INDEX) {
        slide_attackers.clearBit(excluded_index);
    }
    return findCommonBits(slide_attackers, slide_path).hasRemainingBits();
}

bool MoveGenerator::isAttackedByJumpPattern(const Position& pos, BoardIndex target_index, AttackPattern pattern, BoardIndex excluded_index) {
    assert(pattern == AttackPattern::PAWN || pattern == AttackPattern::KNIGHT || pattern == AttackPattern::KING);
    Bitboard jump_origins;
    switch (pattern) {
    case AttackPattern::KNIGHT:
        jump_origins = MoveTables::knight_moves[target_index];
        break;
    case AttackPattern::PAWN:
        jump_origins = MoveTables::pawn_attacks[2 * target_index + (pos.getCurrentColor() == Color::WHITE ? 0 : 1)];
        break;
    case AttackPattern::KING:
        jump_origins = MoveTables::king_moves[target_index];
        break;
    default:
        assert(false);
        break;
    }

    Bitboard attackers = pos.getPiecesByPattern(pos.getOpponentColor(), pattern);
    if (excluded_index != INVALID_INDEX) {
        attackers.clearBit(excluded_index);
    }
    return findCommonBits(jump_origins, attackers).hasRemainingBits();
}

bool MoveGenerator::isAttackedByAnyPattern(const Position& pos, Bitboard targets, Bitboard blockers) {
    if (isAttackedBySlidePattern(pos, targets, AttackPattern::LINE, blockers) ||
        isAttackedBySlidePattern(pos, targets, AttackPattern::DIAGONAL, blockers)) {
        return true;
    }

    BoardIndex target_index;
    Bitboard target = targets.popLowestBit();

    while (target.hasRemainingBits()) {
        target_index = target.singleBitIndex();
        if (isAttackedByJumpPattern(pos, target_index, AttackPattern::KNIGHT) ||
            isAttackedByJumpPattern(pos, target_index, AttackPattern::PAWN) ||
            isAttackedByJumpPattern(pos, target_index, AttackPattern::KING)) {
            return true;
        }
        target = targets.popLowestBit();
    }
    return false;
}

bool MoveGenerator::enemyCheckCheck(const Position& pos, Move proposed_move) {
    Bitboard opponent_king = pos.getPieces(pos.getOpponentColor(), PieceType::KING);
    if (!opponent_king.hasRemainingBits()) {
        return false;
    }

    PieceType moving_type = proposed_move.getAbsoluteMovingType();
    BoardIndex dest_index = proposed_move.getDestinationIndex();
    BoardIndex origin_index = proposed_move.getOriginIndex();

    if (moving_type == PieceType::KNIGHT) {
        if (MoveTables::knight_moves[dest_index].getCommonBitsWith(opponent_king).hasRemainingBits())
            return true;
    } else if (moving_type == PieceType::PAWN) {
        int attack_idx = 2 * dest_index + (pos.getCurrentColor() == Color::WHITE ? 0 : 1);
        if (MoveTables::pawn_attacks[attack_idx].getCommonBitsWith(opponent_king).hasRemainingBits())
            return true;
    }

    Bitboard updated_blockers = pos.getAllPieces();
    updated_blockers.clearBit(origin_index);
    updated_blockers.setBit(dest_index);

    if (proposed_move.isCapture() && proposed_move.getMiscMoveType() == MoveType::PAWN_UNIQE) {
        updated_blockers.clearBit(Position::getEnPassantCaptureLocation(pos.getCurrentColor(), dest_index).singleBitIndex());
    }

    Bitboard line_attackers = pos.getPiecesByPattern(pos.getCurrentColor(), AttackPattern::LINE);
    line_attackers.clearBit(origin_index);
    if (moving_type == PieceType::ROOK || moving_type == PieceType::QUEEN) {
        line_attackers.setBit(dest_index);
    }

    Bitboard line_rays = getSlideDestinations(pos, opponent_king, AttackPattern::LINE, updated_blockers);
    if (line_rays.getCommonBitsWith(line_attackers).hasRemainingBits()) {
        return true;
    }

    Bitboard diag_attackers = pos.getPiecesByPattern(pos.getCurrentColor(), AttackPattern::DIAGONAL);
    diag_attackers.clearBit(origin_index);
    if (moving_type == PieceType::BISHOP || moving_type == PieceType::QUEEN) {
        diag_attackers.setBit(dest_index);
    }

    Bitboard diag_rays = getSlideDestinations(pos, opponent_king, AttackPattern::DIAGONAL, updated_blockers);
    if (diag_rays.getCommonBitsWith(diag_attackers).hasRemainingBits()) {
        return true;
    }

    return false;
}
