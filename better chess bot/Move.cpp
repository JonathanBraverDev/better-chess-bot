#include "Move.h"
#include "Move encoding.h"

class Move {
private:
    uint32_t encodedMove;

public:
    Move() : encodedMove(0) {}
    Move(uint32_t encoded) : encodedMove(encoded) {}

    uint32_t getEncodedMove() const {
        return encodedMove;
    }


    uint8_t getOriginIndex() const {
        return (encodedMove & ORIGIN_INDEX_MASK) >> ORIGIN_INDEX_OFFSET;
    }

    uint8_t getDestinationIndex() const {
        return (encodedMove & DESTINATION_INDEX_MASK) >> DESTINATION_INDEX_OFFSET;
    }

    Color getMoverColor() const {
        return static_cast<Color>((encodedMove & IS_WHITE_MOVE_MASK) >> IS_WHITE_MOVE_OFFSET);
    }

    PieceType getMovingOrPromotedType() const {
        return static_cast<PieceType>((encodedMove & MOVING_TYPE_MASK) >> MOVING_TYPE_OFFSET);
    }

    PieceType getCapturedType() const {
        return static_cast<PieceType>((encodedMove & CAPTURED_TYPE_MASK) >> CAPTURED_TYPE_OFFSET);
    }

    bool isPromotion() const {
        return (encodedMove & IS_PROMOTE_MASK) != 0;
    }

    bool isCheck() const {
        return (encodedMove & IS_CHECK_MASK) != 0;
    }

    bool isCapture() const {
        return (encodedMove & IS_CAPTURE_MASK) != 0;
    }

    int getMiscMoveType() const {
        return (encodedMove & MISC_MOVE_TYPE_MASK) >> MOVE_TYPE_OFFSET;
    }


    // for all setters, wipe the target data with the inverted mask and set the requested value
    void setOriginIndex(uint8_t index) {
        encodedMove = (encodedMove & ~ORIGIN_INDEX_MASK) | ((index << ORIGIN_INDEX_OFFSET) & ORIGIN_INDEX_MASK);
    }

    void setDestinationIndex(uint8_t index) {
        encodedMove = (encodedMove & ~DESTINATION_INDEX_MASK) | ((index << DESTINATION_INDEX_OFFSET) & DESTINATION_INDEX_MASK);
    }

    void setMoverColor(Color color) {
        encodedMove = (encodedMove & ~IS_WHITE_MOVE_MASK) | ((static_cast<uint32_t>(color) << IS_WHITE_MOVE_OFFSET) & IS_WHITE_MOVE_MASK);
    }

    void setMovingOrPromotedType(PieceType type) {
        encodedMove = (encodedMove & ~MOVING_TYPE_MASK) | ((static_cast<uint32_t>(type) << MOVING_TYPE_OFFSET) & MOVING_TYPE_MASK);
    }

    void setCapturedType(PieceType type) {
        encodedMove = (encodedMove & ~CAPTURED_TYPE_MASK) | ((static_cast<uint32_t>(type) << CAPTURED_TYPE_OFFSET) & CAPTURED_TYPE_MASK);
    }

    void setPromotion(bool isPromote) {
        if (isPromote) {
            encodedMove |= IS_PROMOTE_MASK;
        }
        else {
            encodedMove &= ~IS_PROMOTE_MASK;
        }
    }

    void setCheck(bool isCheck) {
        if (isCheck) {
            encodedMove |= IS_CHECK_MASK;
        }
        else {
            encodedMove &= ~IS_CHECK_MASK;
        }
    }

    void setCapture(bool isCapture) {
        if (isCapture) {
            encodedMove |= IS_CAPTURE_MASK;
        }
        else {
            encodedMove &= ~IS_CAPTURE_MASK;
        }
    }

    void setMiscMoveType(int miscType) {
        encodedMove = (encodedMove & ~MISC_MOVE_TYPE_MASK) | ((miscType << MOVE_TYPE_OFFSET) & MISC_MOVE_TYPE_MASK);
    }

};
