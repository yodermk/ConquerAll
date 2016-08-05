#include "game.h"

Game::Game(const Board &iBoard) : board(iBoard), state(State::Initializing)
{

}

void Game::addPlayer(Player iP)
{
    players.push_back(iP);
}

void Game::setFog()
{
    if (state == State::Initializing)
        fog = true;
}

void Game::setTrench()
{
    if (state == State::Initializing)
        trench = true;
}

