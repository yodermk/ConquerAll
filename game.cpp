#include "game.h"

Game::Game(const Board &iBoard) : board(iBoard), state(State::Initializing),
         reinforcements(Reinforcements::Chain),
         sets(Sets::Escalating), initialDeploys(InitialDeploys::Automatic)
{

}

void Game::addPlayer(std::unique_ptr<Player> iP)
{
    players.push_back(std::move(iP));
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

void Game::start()
{
    owner.resize(players.size(), neutral);
    armies.resize(players.size(), 1);

    if (initialDeploys == InitialDeploys::Automatic) {
        // deploy three armies on each territory
        for (int &i : armies)
            i = 3;
    } else {
        // ask players to deploy
    }
}

