#pragma once

#include "Enums.h"
#include "Types.h"

class Move {
private:
    BitMove encodedMove;

public:
    Move();
    Move(BitMove encoded);
    void clear();

    BitMove getEncodedMove() const;

    uint8_t getOriginIndex() const;
    uint8_t getDestinationIndex() const;
    PieceType getMovingOrPromotedType() const;
    AttackerType getAttackerType() const;
    PieceType getCapturedType() const;
    MoveType getMiscMoveType() const;
    bool isCapture() const;
    bool isCheck() const;
    bool isPromotion() const;

    void setOriginIndex(uint8_t index);
    void setDestinationIndex(uint8_t index);
    void setMovingType(PieceType type);
    // Sets promotion flag too
    void setPromotedType(PieceType type);
    // Sets capture flag too
    void setAttackerType(AttackerType type);
    void setCapturedType(PieceType type);
    void setMiscMoveType(MoveType miscType);
    void setCapture(bool isCapture);
    void setCheck(bool isCheck);
    void setPromotion(bool isPromote);

    // check that the move encode is not messed up
    void validate() const;
    // add conversion to text form
};
