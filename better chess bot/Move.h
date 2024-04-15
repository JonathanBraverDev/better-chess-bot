#pragma once

#include "Enums.h"
#include "Types.h"

class Move {
private:
    BitMove encodedMove;

    // leaving potential coding mistakes localzied to the class
    PieceType getMovingOrPromotedType() const;
    AttackerType getAttackerType() const;

    // mask and offset MUST match
    template <typename T>
    void setProperty(BitMove mask, int offset, T value);

public:
    Move();
    Move(BitMove encoded);
    // returns a copy of the move
    Move copy() const;

    // clears most components of the move, leaving BitRights intact
    void clearMoveData();

    // resets ALL componets of the move, use sparingly
    void fullClear();

    BitMove getEncodedMove() const;

    uint8_t getOriginIndex() const;
    uint8_t getDestinationIndex() const;
    PieceType getAbsoluteMovingType() const;
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

    void setWhiteCastleRights(CastleRights castle_rights);
    void setBlackCastleRights(CastleRights castle_rights);
    void setValidEnPassant(bool isValid);
    // index is within the row, 0-7
    void setEnPassantIndex(uint8_t index);

    // add conversion to text form
};

template<typename T>
inline void Move::setProperty(BitMove mask, int offset, T value) {
    encodedMove = (encodedMove & ~mask) | ((static_cast<uint8_t>(value) << offset) & mask);
}