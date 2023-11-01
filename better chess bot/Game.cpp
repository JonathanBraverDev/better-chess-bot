#include "Game.h"

void Game::makeMove() {
    Move move = currentPlayer->findMove(gameState);
    gameState.makeMove(move);
    moveHistory.push_back(move);
    gameHistory.push_back(gameState);

    currentPlayer = (currentPlayer == playerWhite) ? playerBlack : playerWhite;
    moveCounter++;
}
