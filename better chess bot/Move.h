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
    bool isPromotion() const;
    bool isCheck() const;
    bool isCapture() const;
    MoveType getMiscMoveType() const;

    void setOriginIndex(uint8_t index);
    void setDestinationIndex(uint8_t index);
    void setMovingOrPromotedType(PieceType type);
    void setAttackerType(AttackerType type);
    void setCapturedType(PieceType type);
    void setPromotion(bool isPromote);
    void setCheck(bool isCheck);
    void setCapture(bool isCapture);
    void setMiscMoveType(MoveType miscType);

    // add conversion to text form
};
