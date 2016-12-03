#include "player.h"
#include "game.h"

Player::Player(std::string iName) : name(iName)
{
}

std::vector<int> Player::myTerritories()
{
    std::vector<int> terts;
    BoardView bv = g->getBoardView(this);
    for (int i=0; i<bv.size(); i++)
        if (bv[i].first == me)
            terts.push_back(i);
    return terts;
}

