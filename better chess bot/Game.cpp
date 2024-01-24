#include "Game.h"

void Game::playMove() {
    Move move = currentPlayer->pickMove(gameState);
    gameState.makeMove(move);

    currentPlayer = (currentPlayer == playerWhite) ? playerBlack : playerWhite;
}
