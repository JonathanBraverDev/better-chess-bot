#include "Move.h"
#include "Move encoding.h"

// Default constructor initializes data to 0
Move::Move() : encodedMove(0) {}

// Constructor with initial data
Move::Move(BitMove encoded) : encodedMove(encoded) {}


BitMove Move::getEncodedMove() const {
    return encodedMove;
}

uint8_t Move::getOriginIndex() const {
    return (encodedMove & ORIGIN_INDEX_MASK) >> ORIGIN_INDEX_OFFSET;
}

uint8_t Move::getDestinationIndex() const {
    return (encodedMove & DESTINATION_INDEX_MASK) >> DESTINATION_INDEX_OFFSET;
}

Color Move::getMoverColor() const {
    return static_cast<Color>((encodedMove & IS_WHITE_MOVE_MASK) >> IS_WHITE_MOVE_OFFSET);
}

PieceType Move::getMovingOrPromotedType() const {
    return static_cast<PieceType>((encodedMove & MOVING_TYPE_MASK) >> MOVING_TYPE_OFFSET);
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

int Move::getMiscMoveType() const {
    return (encodedMove & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET;
}


// for all setters, wipe the target data with the inverted mask and set the requested value
void Move::setOriginIndex(uint8_t index) {
    encodedMove = (encodedMove & ~ORIGIN_INDEX_MASK) | ((index << ORIGIN_INDEX_OFFSET) & ORIGIN_INDEX_MASK);
}

void Move::setDestinationIndex(uint8_t index) {
    encodedMove = (encodedMove & ~DESTINATION_INDEX_MASK) | ((index << DESTINATION_INDEX_OFFSET) & DESTINATION_INDEX_MASK);
}

void Move::setMoverColor(Color color) {
    encodedMove = (encodedMove & ~IS_WHITE_MOVE_MASK) | ((static_cast<uint32_t>(color) << IS_WHITE_MOVE_OFFSET) & IS_WHITE_MOVE_MASK);
}

void Move::setMovingOrPromotedType(PieceType type) {
    encodedMove = (encodedMove & ~MOVING_TYPE_MASK) | ((static_cast<uint32_t>(type) << MOVING_TYPE_OFFSET) & MOVING_TYPE_MASK);
}

void Move::setCapturedType(PieceType type) {
    encodedMove = (encodedMove & ~CAPTURED_TYPE_MASK) | ((static_cast<uint32_t>(type) << CAPTURED_TYPE_OFFSET) & CAPTURED_TYPE_MASK);
}

void Move::setPromotion(bool isPromote) {
    if (isPromote) {
        encodedMove |= IS_PROMOTE_MASK;
    }
    else {
        encodedMove &= ~IS_PROMOTE_MASK;
    }
}

void Move::setCheck(bool isCheck) {
    if (isCheck) {
        encodedMove |= IS_CHECK_MASK;
    }
    else {
        encodedMove &= ~IS_CHECK_MASK;
    }
}

void Move::setCapture(bool isCapture) {
    if (isCapture) {
        encodedMove |= IS_CAPTURE_MASK;
    }
    else {
        encodedMove &= ~IS_CAPTURE_MASK;
    }
}

void Move::setMiscMoveType(int miscType) {
    encodedMove = (encodedMove & ~MISC_MOVE_TYPE_MASK) | ((miscType << MOVE_TYPE_OFFSET) & MISC_MOVE_TYPE_MASK);
}
