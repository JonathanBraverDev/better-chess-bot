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

uint8_t Move::getOriginIndex() const {
    return (encodedMove & ORIGIN_INDEX_MASK) >> ORIGIN_INDEX_OFFSET;
}

uint8_t Move::getDestinationIndex() const {
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


bool Move::isCapture() const {
    return (encodedMove & IS_CAPTURE_MASK) != 0;
}

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

void Move::setDestinationIndex(uint8_t index) {
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

void Move::setCapture(bool isCapture) {
    if (isCapture) {
        encodedMove |= IS_CAPTURE_MASK;
    } else {
        encodedMove &= ~IS_CAPTURE_MASK;
    }
}

void Move::setCheck(bool isCheck) {
    if (isCheck) {
        encodedMove |= IS_CHECK_MASK;
    } else {
        encodedMove &= ~IS_CHECK_MASK;
    }
}

void Move::setPromotion(bool isPromote) {
    if (isPromote) {
        encodedMove |= IS_PROMOTE_MASK;
    } else {
        encodedMove &= ~IS_PROMOTE_MASK;
    }
}

void Move::setWhiteCastleRights(CastleRights castle_rights) {
    setProperty(WHITE_CASTLE_RIGHTS_MASK, WHITE_CASTLE_RIGHTS_OFFSET, castle_rights);
}

void Move::setBlackCastleRights(CastleRights castle_rights) {
    setProperty(BLACK_CASTLE_RIGHTS_MASK, WHITE_CASTLE_RIGHTS_OFFSET, castle_rights);
}

void Move::setValidEnPassant(bool isValid) {
    if (isValid) {
        encodedMove |= VALID_EN_PASSANT_MASK;
    } else {
        encodedMove &= ~VALID_EN_PASSANT_MASK;
    }
}

void Move::setEnPassantIndex(uint8_t index) {
    assert(index <= 7);
    setProperty(EN_PASSANT_INDEX_MASK, WHITE_CASTLE_RIGHTS_OFFSET, index);
}
