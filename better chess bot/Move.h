#pragma once

#include "Enums.h"
#include <cstdint>

typedef uint32_t BitMove;

class Move {
private:
    BitMove encodedMove;

public:
    Move();
    Move(BitMove encoded);

    BitMove getEncodedMove() const;

    uint8_t getOriginIndex() const;
    uint8_t getDestinationIndex() const;
    Color getMoverColor() const;
    PieceType getMovingOrPromotedType() const;
    PieceType getCapturedType() const;
    bool isPromotion() const;
    bool isCheck() const;
    bool isCapture() const;
    int getMiscMoveType() const;

    void setOriginIndex(uint8_t index);
    void setDestinationIndex(uint8_t index);
    void setMoverColor(Color color);
    void setMovingOrPromotedType(PieceType type);
    void setCapturedType(PieceType type);
    void setPromotion(bool isPromote);
    void setCheck(bool isCheck);
    void setCapture(bool isCapture);
    void setMiscMoveType(int miscType);
};
