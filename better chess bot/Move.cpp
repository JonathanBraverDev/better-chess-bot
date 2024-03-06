#include "Move.h"
#include "MoveEncoding.h"
#include <cassert>

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

PieceType Move::getCapturedType() const {
    PieceType type = static_cast<PieceType>((encodedMove & CAPTURED_TYPE_MASK) >> CAPTURED_TYPE_OFFSET);
    assert(type != PieceType::NONE && type != PieceType::KING);
    return type;
}

MoveType Move::getMiscMoveType() const {
    return static_cast<MoveType>((encodedMove & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET);
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
    encodedMove = (encodedMove & ~ORIGIN_INDEX_MASK) | ((index << ORIGIN_INDEX_OFFSET) & ORIGIN_INDEX_MASK);
}

void Move::setDestinationIndex(uint8_t index) {
    assert(index <= 63);
    encodedMove = (encodedMove & ~DESTINATION_INDEX_MASK) | ((index << DESTINATION_INDEX_OFFSET) & DESTINATION_INDEX_MASK);
}

void Move::setMovingType(PieceType type) {
    assert(type != PieceType::NONE);
    encodedMove = (encodedMove & ~PIECE_TYPE_MASK) | ((static_cast<uint8_t>(type) << PIECE_TYPE_OFFSET) & PIECE_TYPE_MASK);
}

void Move::setPromotedType(PieceType type) {
    assert(type == PieceType::QUEEN || type == PieceType::ROOK || type == PieceType::BISHOP || type == PieceType::KNIGHT);
    setPromotion(true); // can't be a promotion without a... promotion
    encodedMove = (encodedMove & ~PIECE_TYPE_MASK) | ((static_cast<uint8_t>(type) << PIECE_TYPE_OFFSET) & PIECE_TYPE_MASK);
}

void Move::setAttackerType(AttackerType attacker_type) {
    assert(attacker_type != AttackerType::NONE);
    setCapture(true); // can't be an attacker without capture
    encodedMove = (encodedMove & ~PIECE_TYPE_MASK) | ((static_cast<uint8_t>(attacker_type) << PIECE_TYPE_OFFSET) & PIECE_TYPE_MASK);
}

void Move::setCapturedType(PieceType type) {
    assert(type != PieceType::NONE && type != PieceType::KING);
    encodedMove = (encodedMove & ~CAPTURED_TYPE_MASK) | ((static_cast<uint8_t>(type) << CAPTURED_TYPE_OFFSET) & CAPTURED_TYPE_MASK);
}

void Move::setMiscMoveType(MoveType miscType) {
    encodedMove = (encodedMove & ~MISC_MOVE_TYPE_MASK) | ((static_cast<uint8_t>(miscType) << MOVE_TYPE_OFFSET) & MISC_MOVE_TYPE_MASK);
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
