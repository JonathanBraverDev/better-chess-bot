#include "Game.h"

void Game::makeMove() {
    Move move = currentPlayer->findMove(board);
    board.makeMove(move);
    moveHistory.push_back(move);
    boardHistory.push_back(board);

    currentPlayer = (currentPlayer == playerWhite) ? playerBlack : playerWhite;
    moveCounter++;
}
