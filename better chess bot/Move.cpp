#include "Move.h"
#include "MoveEncoding.h"

// Default constructor initializes data to 0
Move::Move() : encodedMove(0) {}

// Constructor with initial data
Move::Move(BitMove encoded) : encodedMove(encoded) {}

void Move::clear() {
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
    return static_cast<PieceType>((encodedMove & MOVING_TYPE_MASK) >> MOVING_TYPE_OFFSET);
}

AttackerType Move::getAttackerType() const {
    return static_cast<AttackerType>((encodedMove & MOVING_TYPE_MASK) >> MOVING_TYPE_OFFSET);
}

PieceType Move::getCapturedType() const {
    return static_cast<PieceType>((encodedMove & CAPTURED_TYPE_MASK) >> CAPTURED_TYPE_OFFSET);
}

bool Move::isPromotion() const {
    return (encodedMove & IS_PROMOTE_MASK) != 0;
}

bool Move::isCheck() const {
    return (encodedMove & IS_CHECK_MASK) != 0;
}

bool Move::isCapture() const {
    return (encodedMove & IS_CAPTURE_MASK) != 0;
}

MoveType Move::getMiscMoveType() const {
    return static_cast<MoveType>((encodedMove & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET);
}


// for all setters, wipe the target data with the inverted mask and set the requested value
void Move::setOriginIndex(uint8_t index) {
    encodedMove = (encodedMove & ~ORIGIN_INDEX_MASK) | ((index << ORIGIN_INDEX_OFFSET) & ORIGIN_INDEX_MASK);
}

void Move::setDestinationIndex(uint8_t index) {
    encodedMove = (encodedMove & ~DESTINATION_INDEX_MASK) | ((index << DESTINATION_INDEX_OFFSET) & DESTINATION_INDEX_MASK);
}

void Move::setMovingOrPromotedType(PieceType type) {
    encodedMove = (encodedMove & ~MOVING_TYPE_MASK) | ((static_cast<uint8_t>(type) << MOVING_TYPE_OFFSET) & MOVING_TYPE_MASK);
}

void Move::setAttackerType(AttackerType attacker_type) {
    encodedMove = (encodedMove & ~MOVING_TYPE_MASK) | ((static_cast<uint8_t>(attacker_type) << MOVING_TYPE_OFFSET) & MOVING_TYPE_MASK);
}

void Move::setCapturedType(PieceType type) {
    encodedMove = (encodedMove & ~CAPTURED_TYPE_MASK) | ((static_cast<uint8_t>(type) << CAPTURED_TYPE_OFFSET) & CAPTURED_TYPE_MASK);
}

void Move::setPromotion(bool isPromote) {
    if (isPromote) {
        encodedMove |= IS_PROMOTE_MASK;
    } else {
        encodedMove &= ~IS_PROMOTE_MASK;
    }
}

void Move::setCheck(bool isCheck) {
    if (isCheck) {
        encodedMove |= IS_CHECK_MASK;
    } else {
        encodedMove &= ~IS_CHECK_MASK;
    }
}

void Move::setCapture(bool isCapture) {
    if (isCapture) {
        encodedMove |= IS_CAPTURE_MASK;
    } else {
        encodedMove &= ~IS_CAPTURE_MASK;
    }
}

void Move::setMiscMoveType(MoveType miscType) {
    encodedMove = (encodedMove & ~MISC_MOVE_TYPE_MASK) | ((static_cast<uint8_t>(miscType) << MOVE_TYPE_OFFSET) & MISC_MOVE_TYPE_MASK);
}
