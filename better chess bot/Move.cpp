#include "Move.h"
#include "MoveEncoding.h"
#include <cassert>

// Default constructor initializes data to 0
Move::Move() : encodedMove(0) {}

// Constructor with initial data
Move::Move(BitMove encoded) : encodedMove(encoded) {}

Move Move::copy() const {
    return Move(encodedMove);
}

void Move::clearMoveData() {
    encodedMove &= BIT_RIGHTS_MASK;
}

void Move::fullClear() {
    encodedMove = 0;
}


BitMove Move::getEncodedMove() const {
    return encodedMove;
}

BoardIndex Move::getOriginIndex() const {
  return (encodedMove & ORIGIN_INDEX_MASK) >> ORIGIN_INDEX_OFFSET;
}

BoardIndex Move::getDestinationIndex() const {
  return (encodedMove & DESTINATION_INDEX_MASK) >> DESTINATION_INDEX_OFFSET;
}

PieceType Move::getMovingOrPromotedType() const {
    PieceType type = static_cast<PieceType>((encodedMove & PIECE_TYPE_MASK) >> PIECE_TYPE_OFFSET);
    assert((!isPromotion() && type != PieceType::NONE) ||
           (isPromotion() && (type == PieceType::QUEEN || type == PieceType::ROOK ||
                              type == PieceType::BISHOP || type == PieceType::KNIGHT)));
    return type;
}

AttackerType Move::getAttackerType() const {
    AttackerType attacker_type = static_cast<AttackerType>((encodedMove & PIECE_TYPE_MASK) >> PIECE_TYPE_OFFSET);
    assert(isCapture() && attacker_type != AttackerType::NONE);
    return attacker_type;
}

// interpret the moving piece data to its "absolute value" piece type
PieceType Move::getAbsoluteMovingType() const {
    if (!isCapture() || isPromotion()) { // promotions use the regular types to aid with move ordering
        return getMovingOrPromotedType();
    } else {
        return attackerTypeToPieceTypeMap.at(getAttackerType());
    }
    return PieceType();
}

PieceType Move::getCapturedType() const {
    PieceType type = static_cast<PieceType>((encodedMove & CAPTURED_TYPE_MASK) >> CAPTURED_TYPE_OFFSET);
    assert(type != PieceType::NONE && type != PieceType::KING);
    return type;
}

MoveType Move::getMiscMoveType() const {
    return static_cast<MoveType>((encodedMove & MISC_MOVE_TYPE_MASK) >> MISC_MOVE_TYPE_OFFSET);
}

bool Move::isCapture() const { return (encodedMove & IS_CAPTURE_MASK) != 0; }

bool Move::isCheck() const {
    return (encodedMove & IS_CHECK_MASK) != 0;
}

bool Move::isPromotion() const {
    return (encodedMove & IS_PROMOTE_MASK) != 0;
}


// for all setters, wipe the target data with the inverted mask and set the requested value
void Move::setOriginIndex(uint8_t index) {
    assert(index <= 63);
    setProperty(ORIGIN_INDEX_MASK, ORIGIN_INDEX_OFFSET, index);
}

void Move::setDestinationIndex(BoardIndex index) {
  assert(index <= 63);
  setProperty(DESTINATION_INDEX_MASK, DESTINATION_INDEX_OFFSET, index);
}

void Move::setMovingType(PieceType type) {
    assert(type != PieceType::NONE);
    setProperty(PIECE_TYPE_MASK, PIECE_TYPE_OFFSET, type);
}

void Move::setPromotedType(PieceType type) {
    assert(type == PieceType::QUEEN || type == PieceType::ROOK ||
           type == PieceType::BISHOP || type == PieceType::KNIGHT);
    setPromotion(true); // can't be a promotion without a... promotion
    setProperty(PIECE_TYPE_MASK, PIECE_TYPE_OFFSET, type);
}

void Move::setAttackerType(AttackerType attacker_type) {
    assert(attacker_type != AttackerType::NONE);
    setCapture(true); // can't be an attacker without capture
    setProperty(PIECE_TYPE_MASK, PIECE_TYPE_OFFSET, attacker_type);
}

void Move::setCapturedType(PieceType type) {
    assert(type != PieceType::NONE && type != PieceType::KING);
    setProperty(CAPTURED_TYPE_MASK, CAPTURED_TYPE_OFFSET, type);
}

void Move::setMiscMoveType(MoveType miscType) {
    setProperty(MISC_MOVE_TYPE_MASK, MISC_MOVE_TYPE_OFFSET, miscType);
}

void Move::setCapture(bool is_capture) {
    if (is_capture) {
        encodedMove |= IS_CAPTURE_MASK;
    } else {
        encodedMove &= ~IS_CAPTURE_MASK;
    }
}

void Move::setCheck(bool is_check) {
    if (is_check) {
        encodedMove |= IS_CHECK_MASK;
    } else {
        encodedMove &= ~IS_CHECK_MASK;
    }
}

void Move::setPromotion(bool is_promote) {
    if (is_promote) {
        encodedMove |= IS_PROMOTE_MASK;
    } else {
        encodedMove &= ~IS_PROMOTE_MASK;
    }
}

void Move::setWhiteShortCastleRight(bool can_castle) {
    if (can_castle) {
        encodedMove |= WHITE_SHORT_CASTLE_MASK;
    } else {
        encodedMove &= ~WHITE_SHORT_CASTLE_MASK;
    }
}

void Move::setWhiteLongCastleRight(bool can_castle) {
    if (can_castle) {
        encodedMove |= WHITE_LONG_CASTLE_MASK;
    } else {
        encodedMove &= ~WHITE_LONG_CASTLE_MASK;
    }
}

void Move::setBlackShortCastleRight(bool can_castle) {
    if (can_castle) {
        encodedMove |= WHITE_LONG_CASTLE_MASK;
    } else {
        encodedMove &= ~WHITE_LONG_CASTLE_MASK;
    }
}

void Move::setBlackLongCastleRight(bool can_castle) {
    if (can_castle) {
        encodedMove |= WHITE_LONG_CASTLE_MASK;
    } else {
        encodedMove &= ~WHITE_LONG_CASTLE_MASK;
    }
}

void Move::setValidEnPassant(bool isValid) {
    if (isValid) {
        encodedMove |= VALID_EN_PASSANT_MASK;
    } else {
        encodedMove &= ~VALID_EN_PASSANT_MASK;
    }
}

void Move::setEnPassantIndex(BoardIndex index) {
  assert(index <= 7);
  setValidEnPassant(true);
  setProperty(EN_PASSANT_INDEX_MASK, EN_PASSANT_INDEX_OFFSET, index);
}


std::string Move::verboseDecode() const {
  if (getMiscMoveType() == MoveType::CASTLE_SHORT) {
    return "Short Castle";
  }
  if (getMiscMoveType() == MoveType::CASTLE_LONG) {
    return "Long Castle";
  }

  std::string origin = tileNames[getOriginIndex()];
  std::string dest = tileNames[getDestinationIndex()];
  std::string description = "";

  if (isPromotion()) {
    description = "Pawn moves from " + origin;
    if (isCapture()) {
      std::string capturedName = pieceTypeToNameMap.at(getCapturedType());
      description += ", captures a " + capturedName + " on " + dest;
    } else {
      description += " to " + dest;
    }
    std::string promotedName = pieceTypeToNameMap.at(getAbsoluteMovingType());
    description += " and promotes to a " + promotedName;
  } else {
    std::string pieceName = pieceTypeToNameMap.at(getAbsoluteMovingType());
    description = pieceName + " moves from " + origin;
    if (isCapture()) {
      std::string capturedName = pieceTypeToNameMap.at(getCapturedType());
      description += " and captures a " + capturedName + " at " + dest;
    } else {
      description += " to " + dest;
    }
  }

  if (isCheck()) {
    description += " with check";
  }

  description += ".";
  return description;
}

