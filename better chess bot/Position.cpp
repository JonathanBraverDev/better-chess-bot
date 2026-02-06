#include "Position.h"
#include "Enums.h"
#include "structs.h"
#include <cassert>

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
  if (move.getMiscMoveType() == MoveType::PAWN_UNIQE && move.isCapture()) {

    Bitboard capture_location(0);
    capture_location.setBit(capture_idx);

    // Shift to find the actual pawn location
    // White ends up above the black pawn when capturing
    if (current_color == Color::WHITE) {
      capture_location.shift(Direction::DOWN);
    } else {
      capture_location.shift(Direction::UP);
    }

    capture_idx = capture_location.singleBitIndex();
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
      Bitboard::combineBoards(own_pieces, opponent_pieces).getInverted();
  Bitboard opponent_en_passant =
      (current_color == Color::WHITE ? BLACK_EN_PASSANT : WHITE_EN_PASSANT);
  Bitboard step;
  Bitboard captures;
  Move move_base = currentBitRights();
  Direction direction_forward =
      (current_color == Color::WHITE ? Direction::UP : Direction::DOWN);
  int color_offset = (current_color == Color::WHITE ? 0 : 1);
  uint8_t pawn_index;
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

    step = Bitboard::findCommonBits(
        precomputed_moves.pawn_moves[pawn_move_index], empty_tiles);
    captures = Bitboard::findCommonBits(
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
    Bitboard jump = Bitboard::findCommonBits(step.look(forward), empty_tiles);

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
      Bitboard::findCommonBits(precomputed_moves.pawn_attacks[pawn_move_index],
                               potential_en_passant, special_move_rights);

  if (en_passant.hasRemainingBits()) {
    move_base.setAttackerType(AttackerType::PAWN);
    move_base.setMiscMoveType(MoveType::PAWN_UNIQE);
    move_base.setCapturedType(PieceType::PAWN);
    move_base.setDestinationIndex(en_passant.singleBitIndex());
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
      destinations = Bitboard::combineBoards(
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
      piece, pattern, Bitboard::combineBoards(own_pieces, opponent_pieces));
}

Bitboard Position::getSlideDestinations(const Bitboard piece,
                                        const AttackPattern pattern,
                                        const Bitboard blockers) const {
  assert(pattern == AttackPattern::DIAGONAL || pattern == AttackPattern::LINE);
  Bitboard destinations;

  switch (pattern) {
  case AttackPattern::DIAGONAL:
    destinations = Bitboard::combineBoards(
        piece.slidePath(Direction::UP_LEFT, blockers),
        piece.slidePath(Direction::UP_RIGHT, blockers),
        piece.slidePath(Direction::DOWN_LEFT, blockers),
        piece.slidePath(Direction::DOWN_RIGHT, blockers));
    break;
  case AttackPattern::LINE:
    destinations =
        Bitboard::combineBoards(piece.slidePath(Direction::UP, blockers),
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
  getCastlingMoves(king, Bitboard::combineBoards(own_pieces, opponent_pieces),
                   move_base);
}

void Position::getCastlingMoves(Bitboard king, Bitboard blockers,
                                Move move_base) const {
  if (!Bitboard::findCommonBits(king, special_move_rights).hasRemainingBits() ||
      isAttackedByAnyPattern(king, getAllPieces())) {
    // No castling rights for the king OR
    // King initially under attack
    return;
  }

  Bitboard own_rooks = getOwnPieces(PieceType::ROOK);
  Bitboard own_castle_row =
      (current_color == Color::WHITE ? WHITE_CASTLE_ROW : BLACK_CASTLE_ROW);
  Bitboard long_rook = Bitboard::findCommonBits(king.lowerThanSingleBit(),
                                                own_rooks, special_move_rights);
  Bitboard short_rook = Bitboard::findCommonBits(
      king.higherThanSingleBit(), own_rooks, special_move_rights);

  Bitboard long_king_dest = Bitboard::findCommonBits(
      own_castle_row, Bitboard(LONG_CASTLE_KING_COLUMN));
  Bitboard long_rook_dest = Bitboard::findCommonBits(
      own_castle_row, Bitboard(LONG_CASTLE_ROOK_COLUMN));
  Bitboard short_king_dest = Bitboard::findCommonBits(
      own_castle_row, Bitboard(SHORT_CASTLE_KING_COLUMN));
  Bitboard short_rook_dest = Bitboard::findCommonBits(
      own_castle_row, Bitboard(SHORT_CASTLE_ROOK_COLUMN));
  move_base = currentBitRights();
  move_base.setOriginIndex(king.singleBitIndex());
  move_base.setMovingType(PieceType::KING);

  // Long castling
  if (canCastle(king, long_rook, long_king_dest, long_rook_dest,
                getAllPieces())) {
    move_base.setDestinationIndex(long_rook.singleBitIndex());
    move_base.setMiscMoveType(MoveType::CASTLE_LONG);

    legal_moves.push_back(move_base);
  }

  // Short castling
  if (canCastle(king, short_rook, short_king_dest, short_rook_dest,
                getAllPieces())) {
    move_base.setDestinationIndex(short_rook.singleBitIndex());
    move_base.setMiscMoveType(MoveType::CASTLE_SHORT);

    legal_moves.push_back(move_base);
  }
}

bool Position::canCastle(const Bitboard king, const Bitboard rook,
                         const Bitboard king_dest, const Bitboard rook_dest,
                         const Bitboard all_pieces) const {
  // Check if the rook can castle, cross ref to rights included in call
  if (!rook.hasRemainingBits()) {
    return false;
  }

  Bitboard king_path = king.sameRowPathTo(king_dest);
  Bitboard rook_path = rook.sameRowPathTo(rook_dest);
  Bitboard castling_pieces = Bitboard::combineBoards(king, rook);
  Bitboard interfering_pieces =
      Bitboard::combineBoards(Bitboard::findCommonBits(all_pieces, rook_path),
                              Bitboard::findCommonBits(all_pieces, king_path));

  // NOTE: does not check the king's origin. that was handled in the caller.
  if (isAttackedByAnyPattern(king_path,
                             all_pieces.getWithoutBitsFrom(castling_pieces))) {
    // The king is passing through or ending in a check
    return false;
  }

  if (interfering_pieces != Bitboard::combineBoards(king, rook)) {
    // A third piece is in the way of either the rook or the king
    return false;
  }

  return true;
}

// filters out 'frienly fire' and saves the moves from the destination board
void Position::finalizeMoves(Bitboard destinations, Move move_base) const {
  destinations.clearBitsFrom(own_pieces);

  Bitboard captures = Bitboard::findCommonBits(destinations, opponent_pieces);
  destinations.clearBitsFrom(captures);

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

  Bitboard king = getPieces(current_color, PieceType::KING);
  uint8_t king_index;
  Bitboard all_pieces = Bitboard::combineBoards(own_pieces, opponent_pieces);

  if (proposed_move.getAbsoluteMovingType() == PieceType::KING) {
    if (isAttackedBySlidePattern(king, AttackPattern::LINE, all_pieces) ||
        isAttackedBySlidePattern(king, AttackPattern::DIAGONAL, all_pieces)) {
      return true;
    }

    king_index = king.singleBitIndex();

    if (isAttackedByJumpPattern(king_index, AttackPattern::KNIGHT) ||
        isAttackedByJumpPattern(king_index, AttackPattern::PAWN) ||
        isAttackedByJumpPattern(king_index, AttackPattern::KING)) {
      return true;
    }

  } else {
    // move the piece blocker to the destination
    all_pieces.clearBit(proposed_move.getOriginIndex());
    all_pieces.setBit(proposed_move.getDestinationIndex());

    // check for criminal negligence (uncovered check)
    if (isAttackedBySlidePattern(king, AttackPattern::LINE, all_pieces) ||
        isAttackedBySlidePattern(king, AttackPattern::DIAGONAL, all_pieces)) {
      return true;
    }
  }

  return false;
}

bool Position::isAttackedBySlidePattern(Bitboard target, AttackPattern pattern,
                                        Bitboard blockers) const {
  assert(pattern == AttackPattern::DIAGONAL || pattern == AttackPattern::LINE);
  Bitboard slide_path = getSlideDestinations(target, pattern, blockers);
  Bitboard slide_attackers = getPiecesByPattern(getOpponentColor(), pattern);
  return Bitboard::findCommonBits(slide_attackers, slide_path)
      .hasRemainingBits();
}

bool Position::isAttackedByJumpPattern(uint8_t target_index,
                                       AttackPattern pattern) const {
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
  return Bitboard::findCommonBits(
             jump_origins, getPiecesByPattern(getOpponentColor(), pattern))
      .hasRemainingBits();
}

bool Position::isAttackedByAnyPattern(Bitboard targets,
                                      Bitboard blockers) const {
  uint8_t target_index;
  Bitboard target = targets.popLowestBit(); // focus on the next target bit

  // Check for sliding piece attacks
  if (isAttackedBySlidePattern(targets, AttackPattern::LINE, blockers) ||
      isAttackedBySlidePattern(targets, AttackPattern::DIAGONAL, blockers)) {
    return true;
  }

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
  // TODO: implement
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
    return Bitboard::combineBoards(getPieces(color, PieceType::BISHOP),
                                   getPieces(color, PieceType::QUEEN));
  case AttackPattern::LINE:
    return Bitboard::combineBoards(getPieces(color, PieceType::ROOK),
                                   getPieces(color, PieceType::QUEEN));
  }
}

Piece Position::getPieceAtIndex(uint8_t index) const {
  Bitboard tile = (1ULL << index);
  return getPieceAtTile(tile);
}

Piece Position::getPieceAtTile(Bitboard tile) const {
  if (Bitboard::findCommonBits(white_pawns, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::PAWN};
  } else if (Bitboard::findCommonBits(black_pawns, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::PAWN};
  } else if (Bitboard::findCommonBits(white_knights, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::KNIGHT};
  } else if (Bitboard::findCommonBits(black_knights, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::KNIGHT};
  } else if (Bitboard::findCommonBits(white_bishops, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::BISHOP};
  } else if (Bitboard::findCommonBits(black_bishops, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::BISHOP};
  } else if (Bitboard::findCommonBits(white_rooks, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::ROOK};
  } else if (Bitboard::findCommonBits(black_rooks, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::ROOK};
  } else if (Bitboard::findCommonBits(white_queens, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::QUEEN};
  } else if (Bitboard::findCommonBits(black_queens, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::QUEEN};
  } else if (Bitboard::findCommonBits(white_king, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::KING};
  } else if (Bitboard::findCommonBits(black_king, tile).hasRemainingBits()) {
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
    own_pieces = Bitboard::combineBoards(
        getOwnPieces(PieceType::PAWN), getOwnPieces(PieceType::KNIGHT),
        getOwnPieces(PieceType::BISHOP), getOwnPieces(PieceType::ROOK),
        getOwnPieces(PieceType::QUEEN), getOwnPieces(PieceType::KING));
  }
  return own_pieces;
}

Bitboard Position::getAllOpponentPieces() const {
  if (opponent_pieces.isEmpty()) {
    opponent_pieces = Bitboard::combineBoards(
        getOpponentPieces(PieceType::PAWN),
        getOpponentPieces(PieceType::KNIGHT),
        getOpponentPieces(PieceType::BISHOP),
        getOpponentPieces(PieceType::ROOK), getOpponentPieces(PieceType::QUEEN),
        getOpponentPieces(PieceType::KING));
  }
  return opponent_pieces;
}

Bitboard Position::getAllPieces() const {
  return Bitboard::combineBoards(getAllOwnPieces(), getAllOpponentPieces());
}

void Position::PrepareKingMoves() {
  Bitboard king = Bitboard(1ULL);
  // It's not ideal that the board is completly detached from the loop...
  for (size_t i = 0; i < 64; i++) {
    precomputed_moves.king_moves[i] = Bitboard::combineBoards(
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
        Bitboard::combineBoards(knight.look(Direction::KNIGHT_UP_LEFT),
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
        Bitboard::combineBoards(white_pawn.look(Direction::UP_LEFT),
                                white_pawn.look(Direction::UP_RIGHT));

    white_pawn.nextTile();
  }
}

void Position::PrepareBlackPawnMoves() {
  Bitboard black_pawn = Bitboard(1ULL);
  for (size_t i = 1; i < 64 * 2; i += 2) {
    precomputed_moves.pawn_moves[i] = black_pawn.look(Direction::DOWN);

    precomputed_moves.pawn_attacks[i] =
        Bitboard::combineBoards(black_pawn.look(Direction::DOWN_LEFT),
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

Move Position::currentBitRights() const {
  Move rights(0);

  // operating directly regardless of color
  if (Bitboard::findCommonBits(white_king, special_move_rights)
          .hasRemainingBits()) {
    rights.setWhiteLongCastleRight(
        Bitboard::findCommonBits(white_king.lowerThanSingleBit(), white_rooks,
                                 special_move_rights)
            .hasRemainingBits());

    rights.setWhiteShortCastleRight(
        Bitboard::findCommonBits(white_king.higherThanSingleBit(), white_rooks,
                                 special_move_rights)
            .hasRemainingBits());
  }

  if (Bitboard::findCommonBits(black_king, special_move_rights)
          .hasRemainingBits()) {
    rights.setBlackLongCastleRight(
        Bitboard::findCommonBits(black_king.lowerThanSingleBit(), black_rooks,
                                 special_move_rights)
            .hasRemainingBits());

    rights.setBlackShortCastleRight(
        Bitboard::findCommonBits(black_king.higherThanSingleBit(), black_rooks,
                                 special_move_rights)
            .hasRemainingBits());
  }

  if (Bitboard::findCommonBits(getOpponentEnPassant(), special_move_rights)
          .hasRemainingBits()) {
    rights.setEnPassantIndex(
        Bitboard::findCommonBits(Bitboard(ALL_EN_PASSANT), special_move_rights)
            .singleBitIndex() %
        8);
  }

  return rights;
}
