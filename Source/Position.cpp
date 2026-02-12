#include "Position.h"
#include "Bitboard.h"
#include "Enums.h"
#include "Structs.h"
#include <cassert>
#include <sstream>
#include <iostream>
#include <string>

// empty initizlization of static member
PrecomputedMoves Position::precomputed_moves;

void Position::makeMove(Move move) {
  if (move.getMiscMoveType() == MoveType::CASTLE_SHORT ||
      move.getMiscMoveType() == MoveType::CASTLE_LONG) {
    toggleCastle(move);
  } else {
    if (move.isCapture()) {
      toggleCaptured(move);
    }

    if (move.isPromotion()) {
      togglePromotion(move);
    } else {
      toggleMove(move);
    }
  }

  updateSpecialMoveRights(move);
  current_color = getOpponentColor();

  // Invalidate cached data
  legal_moves.clear();
  are_moves_valid = false;
  own_pieces.clear();
  opponent_pieces.clear();
}

Bitboard &Position::getPieceBoardRef(Color color, PieceType type) {
  assert(color != Color::NONE && type != PieceType::NONE);
  // Similar to getPieces but returning a reference
  switch (color) {
  case Color::WHITE:
    switch (type) {
    case PieceType::PAWN:
      return white_pawns;
    case PieceType::KNIGHT:
      return white_knights;
    case PieceType::BISHOP:
      return white_bishops;
    case PieceType::ROOK:
      return white_rooks;
    case PieceType::QUEEN:
      return white_queens;
    case PieceType::KING:
      return white_king;
    default:
      break;
    }
    break;
  case Color::BLACK:
    switch (type) {
    case PieceType::PAWN:
      return black_pawns;
    case PieceType::KNIGHT:
      return black_knights;
    case PieceType::BISHOP:
      return black_bishops;
    case PieceType::ROOK:
      return black_rooks;
    case PieceType::QUEEN:
      return black_queens;
    case PieceType::KING:
      return black_king;
    default:
      break;
    }
    break;
  default:
    break;
  }
  assert(false);
  return white_pawns; // This should NEVER happen
}

void Position::toggleCastle(const Move move) {
  Bitboard &king_board = getPieceBoardRef(current_color, PieceType::KING);
  Bitboard &rook_board = getPieceBoardRef(current_color, PieceType::ROOK);

  bool is_short_castle = (move.getMiscMoveType() == MoveType::CASTLE_SHORT);

  // Use pre-calculated masks to toggle both origin and destination in one go
  if (current_color == Color::WHITE) {
    if (is_short_castle) {
      king_board.toggleBitsFrom(Bitboard(WHITE_SHORT_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(WHITE_SHORT_CASTLE_ROOK_MASK));
    } else {
      king_board.toggleBitsFrom(Bitboard(WHITE_LONG_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(WHITE_LONG_CASTLE_ROOK_MASK));
    }
  } else {
    if (is_short_castle) {
      king_board.toggleBitsFrom(Bitboard(BLACK_SHORT_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(BLACK_SHORT_CASTLE_ROOK_MASK));
    } else {
      king_board.toggleBitsFrom(Bitboard(BLACK_LONG_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(BLACK_LONG_CASTLE_ROOK_MASK));
    }
  }

  // todo: support Fischer castling where nither pieceis at the usual position
  // in that case use the move's origin and destination to find pieces
}

void Position::toggleMove(const Move move) {
  Bitboard &piece_board =
      getPieceBoardRef(current_color, move.getAbsoluteMovingType());
  piece_board.toggleBit(move.getOriginIndex());
  piece_board.toggleBit(move.getDestinationIndex());
}

void Position::toggleCaptured(const Move move) {
  Bitboard &captured_board =
      getPieceBoardRef(getOpponentColor(), move.getCapturedType());
  uint8_t capture_idx = move.getDestinationIndex();

  // If En Passant, the captured pawn is not on the destination square
  if (move.isEnPassant()) {

    capture_idx = getEnPassantCaptureLocation(current_color, capture_idx)
                      .singleBitIndex();
  }

  captured_board.toggleBit(capture_idx);
}

void Position::togglePromotion(const Move move) {
  // Remove pawn
  getPieceBoardRef(current_color, PieceType::PAWN)
      .toggleBit(move.getOriginIndex());

  // Add promoted piece
  getPieceBoardRef(current_color, move.getAbsoluteMovingType())
      .toggleBit(move.getDestinationIndex());
}

void Position::updateSpecialMoveRights(const Move move) {
  // Clear existing En Passant rights (always valid for one turn only)
  special_move_rights.clearBitsFrom(Bitboard(ALL_EN_PASSANT));

  switch (move.getAbsoluteMovingType()) {
  case PieceType::KING:
    // If the King makes any move, clear all castle rights
    if (current_color == Color::WHITE) {
      special_move_rights.clearBitsFrom(Bitboard(WHITE_CASTLE_ROW));
    } else {
      special_move_rights.clearBitsFrom(Bitboard(BLACK_CASTLE_ROW));
    }
    break;

  case PieceType::ROOK:
    // If a Rook moves, clear its castling right.
    special_move_rights.clearBit(move.getOriginIndex());
    break;

  case PieceType::PAWN:
    // If a pawn jumps, add an En Passant right behind it
    if (move.getMiscMoveType() == MoveType::PAWN_UNIQE && !move.isCapture()) {
      // Mark the destination tile
      Bitboard ep_location(0);
      ep_location.setBit(move.getDestinationIndex());

      // Shift the marked tile to the correct En Passant position
      // White pawns end up above its En Passant tile when jumping
      if (current_color == Color::WHITE) {
        ep_location.shift(Direction::DOWN);
      } else {
        ep_location.shift(Direction::UP);
      }

      special_move_rights.toggleBitsFrom(ep_location);
    }
    break;

  default:
    break;
  }
}

void Position::getPawnMoves() const {
  Bitboard pawns = getPieces(current_color, PieceType::PAWN);
  Bitboard empty_tiles =
      combineBoards(own_pieces, opponent_pieces).getInverted();
  Bitboard opponent_en_passant =
      (current_color == Color::WHITE ? BLACK_EN_PASSANT : WHITE_EN_PASSANT);
  Bitboard step;
  Bitboard captures;
  Move move_base = currentBitRights();
  Direction direction_forward =
      (current_color == Color::WHITE ? Direction::UP : Direction::DOWN);
  int color_offset = (current_color == Color::WHITE ? 0 : 1);
  BoardIndex pawn_index;
  int pawn_move_index;
  int adjusted_pawn_row;

  Bitboard pawn = pawns.popLowestBit(); // focus on the next piece

  while (pawn.hasRemainingBits()) {
    move_base.clearMoveData();
    step.clear();
    captures.clear();

    pawn_index = pawn.singleBitIndex();
    pawn_move_index = 2 * pawn_index + color_offset;
    adjusted_pawn_row = (current_color == Color::WHITE
                             ? (pawn_index / BOARD_SIZE) + 1
                             : BOARD_SIZE - (pawn_index / BOARD_SIZE));

    step = findCommonBits(
        precomputed_moves.pawn_moves[pawn_move_index], empty_tiles);
    captures = findCommonBits(
        precomputed_moves.pawn_attacks[pawn_move_index], opponent_pieces);

    // the only thing that can be set for sure
    move_base.setOriginIndex(pawn_index);
    switch (adjusted_pawn_row) {
    case PAWN_INITIAL_ROW:
      checkAndAddPawnJump(step, empty_tiles, move_base, direction_forward);
      addNormalPawnMoves(move_base, step, captures);

      break;
    case PAWN_ENPASSANT_ROW:
      checkAndAddEnPassant(opponent_en_passant, pawn_move_index, move_base);
      addNormalPawnMoves(move_base, step, captures);

      break;
    case PAWN_PRE_PROMOTION_ROW:
      addPromotionMoves(step, captures, move_base);
      break;
    default:
      addNormalPawnMoves(move_base, step, captures);
      break;
    }

    pawn = pawns.popLowestBit();
  }
}

void Position::checkAndAddPawnJump(Bitboard step, Bitboard empty_tiles,
                                   Move move_base, Direction forward) const {
  if (step.hasRemainingBits()) {
    // check jump if a step is possible
    Bitboard jump = findCommonBits(step.look(forward), empty_tiles);

    if (jump.hasRemainingBits()) {
      move_base.setMovingType(PieceType::PAWN);
      move_base.setMiscMoveType(MoveType::PAWN_UNIQE);
      move_base.setDestinationIndex(jump.singleBitIndex());

      CheckAndSaveMove(move_base);
    }
  }
}

void Position::checkAndAddEnPassant(Bitboard potential_en_passant,
                                    int pawn_move_index, Move move_base) const {
  // check if en-passant is possible
  Bitboard en_passant =
      findCommonBits(precomputed_moves.pawn_attacks[pawn_move_index],
                               potential_en_passant, special_move_rights);

  if (en_passant.hasRemainingBits()) {
    move_base.setAttackerType(AttackerType::PAWN);
    move_base.setMiscMoveType(MoveType::PAWN_UNIQE);
    move_base.setCapturedType(PieceType::PAWN);
    move_base.setDestinationIndex(en_passant.singleBitIndex());
    CheckAndSaveMove(move_base);
  }
}

void Position::addPromotionMoves(Bitboard step, Bitboard captures,
                                 Move move_base) const {
  if (step.hasRemainingBits()) {
    // add step promotions
    move_base.setDestinationIndex(step.singleBitIndex());

    for (PieceType promotionType : {PieceType::QUEEN, PieceType::ROOK,
                                    PieceType::BISHOP, PieceType::KNIGHT}) {
      move_base.setMovingType(promotionType);
      CheckAndSaveMove(move_base);
    }
  }

  Bitboard promotion = captures.popLowestBit();
  while (promotion.hasRemainingBits()) {
    // add capture promotions
    move_base.setDestinationIndex(promotion.singleBitIndex());
    move_base.setCapturedType(getPieceAtTile(promotion).type);
    move_base.setCapture(true);

    // note that captures do NOT use attacker type to prioritize high promotions
    // this allows sorting by int value but complicates decode a bit
    for (PieceType promotionType : {PieceType::QUEEN, PieceType::ROOK,
                                    PieceType::BISHOP, PieceType::KNIGHT}) {
      move_base.setMovingType(promotionType);
      CheckAndSaveMove(move_base);
    }

    promotion = captures.popLowestBit();
  }
}

void Position::addNormalPawnMoves(Move move_base, Bitboard step,
                                  Bitboard captures) const {
  // base_move is expected to have the origin set
  // note that the passed move is never safely reset

  Bitboard capture = captures.popLowestBit();

  if (step.hasRemainingBits()) {
    // add step
    move_base.setMovingType(PieceType::PAWN);
    move_base.setDestinationIndex(step.singleBitIndex());

    CheckAndSaveMove(move_base);
  }

  while (capture.hasRemainingBits()) {
    // add capture
    move_base.setAttackerType(AttackerType::PAWN);
    move_base.setDestinationIndex(capture.singleBitIndex());
    move_base.setCapturedType(getPieceAtTile(capture).type);

    CheckAndSaveMove(move_base);
    capture = captures.popLowestBit();
  }
}

void Position::getKnightMoves() const {
  Bitboard knights = getPieces(current_color, PieceType::KNIGHT);
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

    finalizeMoves(destinations, move_base);

    knight = knights.popLowestBit();
  }
}

void Position::getSlidingPieceMoves(const PieceType pieceType) const {
  Bitboard pieces = getPieces(current_color, pieceType);
  Bitboard destinations;
  Move move_base;

  Bitboard piece = pieces.popLowestBit(); // focus on the next piece

  while (piece.hasRemainingBits()) {
    move_base.clearMoveData();
    destinations.clear();

    move_base.setMovingType(pieceType);
    move_base.setOriginIndex(piece.singleBitIndex());

    if (pieceType == PieceType::QUEEN) {
      destinations = combineBoards(
          getSlideDestinations(piece, AttackPattern::LINE),
          getSlideDestinations(piece, AttackPattern::DIAGONAL));
    } else if (pieceType == PieceType::ROOK) {
      destinations = getSlideDestinations(piece, AttackPattern::LINE);
    } else if (pieceType == PieceType::BISHOP) {
      destinations = getSlideDestinations(piece, AttackPattern::DIAGONAL);
    }

    destinations.clearBitsFrom(own_pieces);

    finalizeMoves(destinations, move_base);

    piece = pieces.popLowestBit();
  }
}

Bitboard Position::getSlideDestinations(const Bitboard piece,
                                        const AttackPattern pattern) const {
  return getSlideDestinations(
      piece, pattern, combineBoards(own_pieces, opponent_pieces));
}

Bitboard Position::getSlideDestinations(const Bitboard piece,
                                        const AttackPattern pattern,
                                        const Bitboard blockers) const {
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
    destinations =
        combineBoards(piece.slidePath(Direction::UP, blockers),
                                piece.slidePath(Direction::DOWN, blockers),
                                piece.slidePath(Direction::LEFT, blockers),
                                piece.slidePath(Direction::RIGHT, blockers));
    break;
  }
  return destinations;
}

void Position::getBishopMoves() const {
  getSlidingPieceMoves(PieceType::BISHOP);
}

void Position::getRookMoves() const { getSlidingPieceMoves(PieceType::ROOK); }

void Position::getQueenMoves() const { getSlidingPieceMoves(PieceType::QUEEN); }

void Position::getKingMoves() const {
  Bitboard king = getPieces(current_color, PieceType::KING);
  Bitboard own_pieces = getAllOwnPieces();
  Bitboard opponent_pieces = getAllOpponentPieces();
  Bitboard destinations;
  Move move_base;

  move_base.setMovingType(PieceType::KING);
  move_base.setOriginIndex(king.singleBitIndex());

  destinations = precomputed_moves.king_moves[king.singleBitIndex()];
  destinations.clearBitsFrom(own_pieces);

  finalizeMoves(destinations, move_base);

  // add castles
  getCastlingMoves(king, combineBoards(own_pieces, opponent_pieces),
                   move_base);
}

void Position::getCastlingMoves(Bitboard king, Bitboard blockers,
                                Move move_base) const {
  if (!findCommonBits(king, special_move_rights).hasRemainingBits() ||
      isAttackedByAnyPattern(king, getAllPieces())) {
    // No castling rights for the king OR
    // King initially under attack
    return;
  }

  Bitboard own_rooks = getOwnPieces(PieceType::ROOK);
  Bitboard own_castle_row =
      (current_color == Color::WHITE ? WHITE_CASTLE_ROW : BLACK_CASTLE_ROW);
  Bitboard long_rook = findCommonBits(king.lowerThanSingleBit(),
                                                own_rooks, special_move_rights, own_castle_row);
  Bitboard short_rook = findCommonBits(
      king.higherThanSingleBit(), own_rooks, special_move_rights, own_castle_row);

  Bitboard long_king_dest = findCommonBits(
      own_castle_row, Bitboard(LONG_CASTLE_KING_COLUMN));
  Bitboard long_rook_dest = findCommonBits(
      own_castle_row, Bitboard(LONG_CASTLE_ROOK_COLUMN));
  Bitboard short_king_dest = findCommonBits(
      own_castle_row, Bitboard(SHORT_CASTLE_KING_COLUMN));
  Bitboard short_rook_dest = findCommonBits(
      own_castle_row, Bitboard(SHORT_CASTLE_ROOK_COLUMN));
  move_base = currentBitRights();
  move_base.setOriginIndex(king.singleBitIndex());
  move_base.setMovingType(PieceType::KING);

  // Long castling
  if (long_rook.hasRemainingBits() &&
      canCastleWithRook(king, long_rook, long_king_dest, long_rook_dest)) {
    move_base.setDestinationIndex(long_rook.singleBitIndex());
    move_base.setMiscMoveType(MoveType::CASTLE_LONG);

    legal_moves.push_back(move_base);
  }

  // Short castling
  if (short_rook.hasRemainingBits() &&
      canCastleWithRook(king, short_rook, short_king_dest, short_rook_dest)) {
    move_base.setDestinationIndex(short_rook.singleBitIndex());
    move_base.setMiscMoveType(MoveType::CASTLE_SHORT);

    legal_moves.push_back(move_base);
  }
}

bool Position::canCastleWithRook(const Bitboard king, const Bitboard rook,
                                 const Bitboard king_dest, const Bitboard rook_dest) const {
  // Check if the rook can castle, cross ref to rights included in call
  if (!rook.getCommonBitsWith(special_move_rights).hasRemainingBits()) {
    return false;
  }

  Bitboard all_pieces = getAllPieces();
  Bitboard king_path = king.sameRowPathTo(king_dest);
  Bitboard rook_path = rook.sameRowPathTo(rook_dest);
  Bitboard castling_pieces = combineBoards(king, rook);
  Bitboard blockers = all_pieces.getWithoutBitsFrom(castling_pieces);

  // NOTE: does not check the king's origin. was handled in getCastlingMoves.
  if (isAttackedByAnyPattern(king_path, blockers)) {
    // The king is passing through or ending in a check
    return false;
  }

  Bitboard interfering_pieces =
      combineBoards(findCommonBits(blockers, rook_path),
                              findCommonBits(blockers, king_path));

  if (interfering_pieces.hasRemainingBits()) {
    // A third piece is in the way of either the rook or the king
    return false;
  }

  return true;
}

// filters out 'frienly fire' and saves the moves from the destination board
void Position::finalizeMoves(Bitboard destinations, Move move_base) const {
  destinations.clearBitsFrom(own_pieces);

  Bitboard captures = findCommonBits(destinations, opponent_pieces);
  destinations.clearBitsFrom(captures);
  captures.clearBitsFrom(getPieces(getOpponentColor(), PieceType::KING)); // ignore captures of the king

  addDestinationMoves(destinations, move_base);
  move_base.setAttackerType(
      pieceTypeToAttackerMap.at(move_base.getAbsoluteMovingType()));
  addCaptureMoves(captures, move_base);
}

void Position::addDestinationMoves(Bitboard destinations,
                                   Move move_base) const {
  Bitboard destination = destinations.popLowestBit();
  Move move;

  while (destination.hasRemainingBits()) {
    move = move_base; // reset the move

    move.setDestinationIndex(destination.singleBitIndex());
    CheckAndSaveMove(move);

    destination = destinations.popLowestBit();
  }
}

void Position::addCaptureMoves(Bitboard captures, Move move_base) const {
  Bitboard capture = captures.popLowestBit();
  Move move;

  while (capture.hasRemainingBits()) {
    move = move_base; // reset the move

    move.setDestinationIndex(capture.singleBitIndex());
    move.setCapturedType(getPieceAtTile(capture).type);
    move.setCapture(true); // very similar functions to "DestinationMoves" but
                           // saves the check on every tile

    CheckAndSaveMove(move);

    capture = captures.popLowestBit();
  }
}

// saves only legal moves, updates flags
// use this instead of directly pusning to vector
void Position::CheckAndSaveMove(Move proposed_move) const {
  if (!selfCheckCheck(proposed_move)) {
    // move is legal, run additional flag checks like check and FREE FLAG

    if (enemyCheckCheck(proposed_move)) {
      proposed_move.setCheck(true);
    }

    legal_moves.push_back(proposed_move);
  }
}

// check if the move results in a self check
bool Position::selfCheckCheck(Move proposed_move) const {

  Bitboard own_king = getPieces(current_color, PieceType::KING);
  BoardIndex king_index;
  Bitboard blockers = combineBoards(own_pieces, opponent_pieces);

  // move the piece blocker to the destination
  blockers.clearBit(proposed_move.getOriginIndex());
  blockers.setBit(proposed_move.getDestinationIndex());

  // for captures, exclude the captured piece from the attacker set
  // since it is still technically on the piece boards
  BoardIndex capture_excluded = INVALID_INDEX;
  if (proposed_move.isCapture()) {
    capture_excluded = proposed_move.getDestinationIndex();
  }

  if (proposed_move.getAbsoluteMovingType() == PieceType::KING) {
    // update king position for check detection
    own_king.clear();
    own_king.setBit(proposed_move.getDestinationIndex());
    king_index = proposed_move.getDestinationIndex();

    if (isAttackedBySlidePattern(own_king, AttackPattern::LINE, blockers, capture_excluded) ||
        isAttackedBySlidePattern(own_king, AttackPattern::DIAGONAL, blockers, capture_excluded)) {
      return true;
    }

    if (isAttackedByJumpPattern(king_index, AttackPattern::KNIGHT, capture_excluded) ||
        isAttackedByJumpPattern(king_index, AttackPattern::PAWN, capture_excluded) ||
        isAttackedByJumpPattern(king_index, AttackPattern::KING, capture_excluded)) {
      return true;
    }

  } else {
    king_index = own_king.singleBitIndex();

    // handle en passant: remove the captured pawn from blockers
    if (proposed_move.isEnPassant()) {
      blockers.clearBitsFrom(getEnPassantCaptureLocation(current_color, proposed_move.getDestinationIndex()));
    }

    // check for discovered check from sliding pieces
    if (isAttackedBySlidePattern(own_king, AttackPattern::LINE, blockers, capture_excluded) ||
        isAttackedBySlidePattern(own_king, AttackPattern::DIAGONAL, blockers, capture_excluded)) {
      return true;
    }

    // if currently in check and not moving the king
    // look for static jump checks and uncovered sliding checks
    if (isInCheck()) {
        BoardIndex jump_excluded = capture_excluded;
        if (proposed_move.isEnPassant()) {
             // for en passant, the captured pawn is not at the destination
             jump_excluded = getEnPassantCaptureLocation(current_color, proposed_move.getDestinationIndex()).singleBitIndex();
        }

        if (isAttackedByJumpPattern(king_index, AttackPattern::KNIGHT, jump_excluded) ||
            isAttackedByJumpPattern(king_index, AttackPattern::PAWN, jump_excluded)) {
          return true;
        }
    }
  }

  return false;
}

bool Position::isAttackedBySlidePattern(Bitboard target, AttackPattern pattern,
                                        Bitboard blockers,
                                        BoardIndex excluded_index) const {
  assert(pattern == AttackPattern::DIAGONAL || pattern == AttackPattern::LINE);
  Bitboard slide_path = getSlideDestinations(target, pattern, blockers);
  Bitboard slide_attackers = getPiecesByPattern(getOpponentColor(), pattern);
  if (excluded_index != INVALID_INDEX) {
    slide_attackers.clearBit(excluded_index);
  }
  return findCommonBits(slide_attackers, slide_path)
      .hasRemainingBits();
}



bool Position::isAttackedByJumpPattern(BoardIndex target_index,
                                       AttackPattern pattern,
                                       BoardIndex excluded_index) const {
  assert(pattern == AttackPattern::PAWN || pattern == AttackPattern::KNIGHT ||
         pattern == AttackPattern::KING);
  Bitboard jump_origins;
  switch (pattern) {
  case AttackPattern::KNIGHT:
    jump_origins = precomputed_moves.knight_moves[target_index];
    break;
  case AttackPattern::PAWN:
    jump_origins =
        precomputed_moves.pawn_attacks[2 * target_index +
                                       (current_color == Color::WHITE ? 0 : 1)];
    break;
  case AttackPattern::KING:
    jump_origins = precomputed_moves.king_moves[target_index];
    break;
  default:
    assert(false);
    break;
  }

  Bitboard attackers = getPiecesByPattern(getOpponentColor(), pattern);
  if (excluded_index != INVALID_INDEX) {
      attackers.clearBit(excluded_index);
  }
  
  return findCommonBits(jump_origins, attackers).hasRemainingBits();
}

bool Position::isAttackedByAnyPattern(Bitboard targets,
                                      Bitboard blockers) const {
  // Check for sliding piece attacks (on all targets at once)
  if (isAttackedBySlidePattern(targets, AttackPattern::LINE, blockers) ||
      isAttackedBySlidePattern(targets, AttackPattern::DIAGONAL, blockers)) {
    return true;
  }

  BoardIndex target_index;
  Bitboard target = targets.popLowestBit(); // focus on the next target bit

  while (target.hasRemainingBits()) {
    // Get the index of the current target
    target_index = target.singleBitIndex();

    // Check for jumping piece attacks
    if (isAttackedByJumpPattern(target_index, AttackPattern::KNIGHT) ||
        isAttackedByJumpPattern(target_index, AttackPattern::PAWN) ||
        isAttackedByJumpPattern(target_index, AttackPattern::KING)) {
      return true;
    }

    // Focus on the next target bit
    target = targets.popLowestBit();
  }

  // If none of the target positions are under attack, return false
  return false;
}

// checks if the move puts the enemy in check
bool Position::enemyCheckCheck(Move proposed_move) const {
  Bitboard opponent_king = getPieces(getOpponentColor(), PieceType::KING);

  if (!opponent_king.hasRemainingBits()) {
    return false;
  }

  // Use the type that will be on the board (promoted or moving)
  PieceType moving_type = proposed_move.getAbsoluteMovingType();
  BoardIndex dest_index = proposed_move.getDestinationIndex();
  BoardIndex origin_index = proposed_move.getOriginIndex();

  // 1. Direct Check (Jumpers)
  // These are unaffected by blockers, so we check them immediately for speed.
  if (moving_type == PieceType::KNIGHT) {
    if (precomputed_moves.knight_moves[dest_index]
            .getCommonBitsWith(opponent_king)
            .hasRemainingBits())
      return true;
  } else if (moving_type == PieceType::PAWN) {
    // Check if Pawn at destination attacks King
    // Pawns capture diagonally forward relative to their color
    int attack_idx = 2 * dest_index + (current_color == Color::WHITE ? 0 : 1);
    if (precomputed_moves.pawn_attacks[attack_idx]
            .getCommonBitsWith(opponent_king)
            .hasRemainingBits())
      return true;
  }

  // Update the blocker board
  Bitboard updated_blockers = getAllPieces();
  updated_blockers.clearBit(origin_index);
  updated_blockers.setBit(dest_index);

  // Handle Capture (mask out captured piece)
  if (proposed_move.isCapture() &&
      proposed_move.getMiscMoveType() == MoveType::PAWN_UNIQE) {
    updated_blockers.clearBit(
        getEnPassantCaptureLocation(current_color, dest_index)
            .singleBitIndex());
  }

  // Search for attackers FROM the enemy king

  // -- LINE CHECKS --
  Bitboard line_attackers =
      getPiecesByPattern(current_color, AttackPattern::LINE);
  // remove piece from origin, add to destination if a line attacker moved
  line_attackers.clearBit(origin_index);
  if (moving_type == PieceType::ROOK || moving_type == PieceType::QUEEN) {
    line_attackers.setBit(dest_index);
  }

  Bitboard line_rays = getSlideDestinations(opponent_king, AttackPattern::LINE,
                                            updated_blockers);
  if (line_rays.getCommonBitsWith(line_attackers).hasRemainingBits()) {
    return true;
  }

  // -- DIAGONAL CHECKS --
  Bitboard diag_attackers =
      getPiecesByPattern(current_color, AttackPattern::DIAGONAL);
  // remove piece from origin, add to destination if a diagonal attacker moved
  diag_attackers.clearBit(origin_index);
  if (moving_type == PieceType::BISHOP || moving_type == PieceType::QUEEN) {
    diag_attackers.setBit(dest_index);
  }

  Bitboard diag_rays = getSlideDestinations(
      opponent_king, AttackPattern::DIAGONAL, updated_blockers);
  if (diag_rays.getCommonBitsWith(diag_attackers).hasRemainingBits()) {
    return true;
  }

  return false;
}

Color Position::getOpponentColor() const {
  return (current_color == Color::WHITE ? Color::BLACK : Color::WHITE);
}

Bitboard Position::getOpponentEnPassant() const {
  return Bitboard(
      (current_color == Color::WHITE ? BLACK_EN_PASSANT : WHITE_EN_PASSANT));
}

Bitboard Position::getPieces(Color color, PieceType type) const {
  assert(color != Color::NONE && type != PieceType::NONE);
  switch (color) {
  case Color::WHITE:
    switch (type) {
    case PieceType::PAWN:
      return white_pawns;
    case PieceType::KNIGHT:
      return white_knights;
    case PieceType::BISHOP:
      return white_bishops;
    case PieceType::ROOK:
      return white_rooks;
    case PieceType::QUEEN:
      return white_queens;
    case PieceType::KING:
      return white_king;
    }
    break;
  case Color::BLACK:
    switch (type) {
    case PieceType::PAWN:
      return black_pawns;
    case PieceType::KNIGHT:
      return black_knights;
    case PieceType::BISHOP:
      return black_bishops;
    case PieceType::ROOK:
      return black_rooks;
    case PieceType::QUEEN:
      return black_queens;
    case PieceType::KING:
      return black_king;
    }
    break;
  }
  assert(false);
  return Bitboard(0);
}

Bitboard Position::getPiecesByPattern(Color color,
                                      AttackPattern pattern) const {
  switch (pattern) {
  case AttackPattern::PAWN:
    return getPieces(color, PieceType::PAWN);
  case AttackPattern::KNIGHT:
    return getPieces(color, PieceType::KNIGHT);
  case AttackPattern::KING:
    return getPieces(color, PieceType::KING);
  case AttackPattern::DIAGONAL:
    return combineBoards(getPieces(color, PieceType::BISHOP),
                                   getPieces(color, PieceType::QUEEN));
  case AttackPattern::LINE:
    return combineBoards(getPieces(color, PieceType::ROOK),
                                   getPieces(color, PieceType::QUEEN));
  }
}

Piece Position::getPieceAtIndex(BoardIndex index) const {
  Bitboard tile = (1ULL << index);
  return getPieceAtTile(tile);
}

Piece Position::getPieceAtTile(Bitboard tile) const {
  if (findCommonBits(white_pawns, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::PAWN};
  } else if (findCommonBits(black_pawns, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::PAWN};
  } else if (findCommonBits(white_knights, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::KNIGHT};
  } else if (findCommonBits(black_knights, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::KNIGHT};
  } else if (findCommonBits(white_bishops, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::BISHOP};
  } else if (findCommonBits(black_bishops, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::BISHOP};
  } else if (findCommonBits(white_rooks, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::ROOK};
  } else if (findCommonBits(black_rooks, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::ROOK};
  } else if (findCommonBits(white_queens, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::QUEEN};
  } else if (findCommonBits(black_queens, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::QUEEN};
  } else if (findCommonBits(white_king, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::KING};
  } else if (findCommonBits(black_king, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::KING};
  } else {
    return {Color::NONE, PieceType::NONE};
  }
}

std::vector<Move> Position::getLegalMoves() const {
  if (!are_moves_valid) {
    getAllOwnPieces();
    getAllOpponentPieces();

    getPawnMoves();
    getKnightMoves();
    getBishopMoves();
    getRookMoves();
    getQueenMoves();
    getKingMoves();

    are_moves_valid = true;
  }

  return legal_moves;
}

Bitboard Position::getOwnPieces(PieceType type) const {
  return getPieces(current_color, type);
}

Bitboard Position::getOpponentPieces(PieceType type) const {
  return getPieces(getOpponentColor(), type);
}

Bitboard Position::getAllOwnPieces() const {
  if (own_pieces.isEmpty()) {
    own_pieces = combineBoards(
        getOwnPieces(PieceType::PAWN), getOwnPieces(PieceType::KNIGHT),
        getOwnPieces(PieceType::BISHOP), getOwnPieces(PieceType::ROOK),
        getOwnPieces(PieceType::QUEEN), getOwnPieces(PieceType::KING));
  }
  return own_pieces;
}

Bitboard Position::getAllOpponentPieces() const {
  if (opponent_pieces.isEmpty()) {
    opponent_pieces = combineBoards(
        getOpponentPieces(PieceType::PAWN),
        getOpponentPieces(PieceType::KNIGHT),
        getOpponentPieces(PieceType::BISHOP),
        getOpponentPieces(PieceType::ROOK), getOpponentPieces(PieceType::QUEEN),
        getOpponentPieces(PieceType::KING));
  }
  return opponent_pieces;
}

Bitboard Position::getAllPieces() const {
  return combineBoards(getAllOwnPieces(), getAllOpponentPieces());
}

void Position::PrepareKingMoves() {
  Bitboard king = Bitboard(1ULL);
  // It's not ideal that the board is completly detached from the loop...
  for (size_t i = 0; i < 64; i++) {
    precomputed_moves.king_moves[i] = combineBoards(
        king.look(Direction::UP), king.look(Direction::DOWN),
        king.look(Direction::LEFT), king.look(Direction::RIGHT),
        king.look(Direction::UP_LEFT), king.look(Direction::UP_RIGHT),
        king.look(Direction::DOWN_LEFT), king.look(Direction::DOWN_RIGHT));

    king.nextTile();
  }
}

void Position::PrepareKnightMoves() {
  Bitboard knight = Bitboard(1ULL);
  for (size_t i = 0; i < 64; i++) {
    precomputed_moves.knight_moves[i] =
        combineBoards(knight.look(Direction::KNIGHT_UP_LEFT),
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

    precomputed_moves.pawn_attacks[i] =
        combineBoards(white_pawn.look(Direction::UP_LEFT),
                                white_pawn.look(Direction::UP_RIGHT));

    white_pawn.nextTile();
  }
}

void Position::PrepareBlackPawnMoves() {
  Bitboard black_pawn = Bitboard(1ULL);
  for (size_t i = 1; i < 64 * 2; i += 2) {
    precomputed_moves.pawn_moves[i] = black_pawn.look(Direction::DOWN);

    precomputed_moves.pawn_attacks[i] =
        combineBoards(black_pawn.look(Direction::DOWN_LEFT),
                                black_pawn.look(Direction::DOWN_RIGHT));

    black_pawn.nextTile();
  }
}

// finds the location of the pawn that was captured by en passant
Bitboard Position::getEnPassantCaptureLocation(Color capturing_color,
                                      BoardIndex en_passant_tile_index) {
  Bitboard capture_location(0);
  capture_location.setBit(en_passant_tile_index);

  // Shift to find the actual pawn location
  // White ends up above the black pawn when capturing
  if (capturing_color == Color::WHITE) {
    capture_location.shift(Direction::DOWN);
  } else {
    capture_location.shift(Direction::UP);
  }

  return capture_location;
}

void Position::InitializeMoves() {
  PrepareKingMoves();
  PrepareKnightMoves();
  PrepareWhitePawnMoves();
  PrepareBlackPawnMoves();
}

bool Position::isInCheck() const {
  Bitboard king = getPieces(current_color, PieceType::KING);
  return isAttackedByAnyPattern(king, getAllPieces());
}

Move Position::currentBitRights() const {
  Move rights(0);

  // operating directly regardless of color
  if (findCommonBits(white_king, special_move_rights)
          .hasRemainingBits()) {
    rights.setWhiteLongCastleRight(
        findCommonBits(white_king.lowerThanSingleBit(), white_rooks,
                                 special_move_rights)
            .hasRemainingBits());

    rights.setWhiteShortCastleRight(
        findCommonBits(white_king.higherThanSingleBit(), white_rooks,
                                 special_move_rights)
            .hasRemainingBits());
  }

  if (findCommonBits(black_king, special_move_rights)
          .hasRemainingBits()) {
    rights.setBlackLongCastleRight(
        findCommonBits(black_king.lowerThanSingleBit(), black_rooks,
                                 special_move_rights)
            .hasRemainingBits());

    rights.setBlackShortCastleRight(
        findCommonBits(black_king.higherThanSingleBit(), black_rooks,
                                 special_move_rights)
            .hasRemainingBits());
  }

  if (findCommonBits(getOpponentEnPassant(), special_move_rights)
          .hasRemainingBits()) {
    rights.setEnPassantIndex(
        findCommonBits(Bitboard(ALL_EN_PASSANT), special_move_rights)
            .singleBitIndex() %
        8);
  }

  return rights;
}

Position::Position() {
  white_pawns.clear();
  white_knights.clear();
  white_bishops.clear();
  white_rooks.clear();
  white_queens.clear();
  white_king.clear();
  black_pawns.clear();
  black_knights.clear();
  black_bishops.clear();
  black_rooks.clear();
  black_queens.clear();
  black_king.clear();
  special_move_rights.clear();
  own_pieces.clear();
  opponent_pieces.clear();
  legal_moves.clear();
  are_moves_valid = false;
  current_color = Color::WHITE;
}

Position Position::fromFen(FenString fen) {
  Position pos;

  std::stringstream ss(fen);
  std::string segment;

  // Piece Placement
  if (std::getline(ss, segment, ' ')) {
    int rank = BOARD_SIZE - 1;
    int file = 0;

    for (char c : segment) {
      if (c == '/') {
        rank--;
        file = 0;
      } else if (isdigit(c)) {
        file += c - '0';
      } else {
        BoardIndex index = rank * BOARD_SIZE + file;

        switch (c) {
        case 'P':
          pos.white_pawns.setBit(index);
          break;
        case 'N':
          pos.white_knights.setBit(index);
          break;
        case 'B':
          pos.white_bishops.setBit(index);
          break;
        case 'R':
          pos.white_rooks.setBit(index);
          break;
        case 'Q':
          pos.white_queens.setBit(index);
          break;
        case 'K':
          pos.white_king.setBit(index);
          break;
        case 'p':
          pos.black_pawns.setBit(index);
          break;
        case 'n':
          pos.black_knights.setBit(index);
          break;
        case 'b':
          pos.black_bishops.setBit(index);
          break;
        case 'r':
          pos.black_rooks.setBit(index);
          break;
        case 'q':
          pos.black_queens.setBit(index);
          break;
        case 'k':
          pos.black_king.setBit(index);
          break;
        }
        file++;
      }
    }
  }

  // Active Color
  if (std::getline(ss, segment, ' ')) {
    pos.current_color = (segment == "w") ? Color::WHITE : Color::BLACK;
  } else {
    pos.current_color = Color::WHITE; // Default
  }

  // Castling Rights
  if (std::getline(ss, segment, ' ')) {
    if (segment != "-") {

      for (char c : segment) {
        switch (c) {
        case 'K':
          pos.special_move_rights.setBit(E1_index);
          pos.special_move_rights.setBit(H1_index);
          break;
        case 'Q':
          pos.special_move_rights.setBit(E1_index);
          pos.special_move_rights.setBit(A1_index);
          break;
        case 'k':
          pos.special_move_rights.setBit(E8_index);
          pos.special_move_rights.setBit(H8_index);
          break;
        case 'q':
          pos.special_move_rights.setBit(E8_index);
          pos.special_move_rights.setBit(A8_index);
          break;
        }
      }
    }
  }

  // En Passant Target
  if (std::getline(ss, segment, ' ')) {
    if (segment != "-") {
      // segment is like "e3"
      int file = segment[0] - 'a';
      int rank = segment[1] - '1';
      BoardIndex ep_index = rank * BOARD_SIZE + file;

      pos.special_move_rights.setBit(ep_index);
    }
  }

  // Fill cache
  pos.own_pieces = pos.getAllOwnPieces();
  pos.opponent_pieces = pos.getAllOpponentPieces();

  return pos;
}

std::string Position::toFen() const {
  std::stringstream ss;

  // 1. Piece Placement
  for (int rank = BOARD_SIZE - 1; rank >= 0; --rank) {
    int empty_count = 0;
    for (int file = 0; file < BOARD_SIZE; ++file) {
      BoardIndex index = rank * BOARD_SIZE + file;
      Piece piece = getPieceAtIndex(index);

      if (piece.type == PieceType::NONE) {
        empty_count++;
      } else {
        if (empty_count > 0) {
          ss << empty_count;
          empty_count = 0;
        }

        char p = '?';
        switch (piece.type) {
        case PieceType::PAWN:   p = 'p'; break;
        case PieceType::KNIGHT: p = 'n'; break;
        case PieceType::BISHOP: p = 'b'; break;
        case PieceType::ROOK:   p = 'r'; break;
        case PieceType::QUEEN:  p = 'q'; break;
        case PieceType::KING:   p = 'k'; break;
        }

        if (piece.color == Color::WHITE) {
          p = toupper(p);
        }
        ss << p;
      }
    }
    if (empty_count > 0) {
      ss << empty_count;
    }
    if (rank > 0) {
      ss << '/';
    }
  }

  // 2. Active Color
  ss << " " << (current_color == Color::WHITE ? "w" : "b");

  // 3. Castling Rights
  ss << " ";
  bool castling_available = false;
  if (special_move_rights.getBit(E1_index) && special_move_rights.getBit(H1_index)) {
      ss << "K"; castling_available = true;
  }
  if (special_move_rights.getBit(E1_index) && special_move_rights.getBit(A1_index)) {
      ss << "Q"; castling_available = true;
  }
  if (special_move_rights.getBit(E8_index) && special_move_rights.getBit(H8_index)) {
      ss << "k"; castling_available = true;
  }
  if (special_move_rights.getBit(E8_index) && special_move_rights.getBit(A8_index)) {
      ss << "q"; castling_available = true;
  }
  if (!castling_available) {
    ss << "-";
  }

  // 4. En Passant Target
  ss << " ";
  Bitboard ep_board = special_move_rights.getCommonBitsWith(Bitboard(ALL_EN_PASSANT));
  if (ep_board.hasRemainingBits()) {
    BoardIndex ep_index = ep_board.singleBitIndex();
    int file = ep_index % BOARD_SIZE;
    int rank = ep_index / BOARD_SIZE;
    char file_char = 'a' + file;
    char rank_char = '1' + rank;
    ss << file_char << rank_char;
  } else {
    ss << "-";
  }

  // 5. Halfmove Clock & 6. Fullmove Number (dummies)
  ss << " 0 1";

  return ss.str();
}
