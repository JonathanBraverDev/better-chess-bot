#pragma once

#include "Board.h"
#include "Move.h"

class Player {
private:
    Color color; // to make each player 'aware' of thier own pieces
    int evaluationParameter; // very much a placeholder for a WAY more complex... something
public:
    virtual Move findMove(const Board& board) = 0; // not quite sure how i want to handle eval at the moment
};
