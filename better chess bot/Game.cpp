#include "Game.h"

void Game::playMove() {
    Move move = currentPlayer->pickMove(gameState);
    gameState.makeMove(move);
    moveHistory.push_back(move);
    gameHistory.push_back(gameState);

    currentPlayer = (currentPlayer == playerWhite) ? playerBlack : playerWhite;
    moveCounter++;
}
