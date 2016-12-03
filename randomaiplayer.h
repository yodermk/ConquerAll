#pragma once

#include "basicaiplayer.h"

// a player whose every move is completely random!

class RandomAIPlayer : public BasicAIPlayer
{
public:
    RandomAIPlayer(std::string iName);
    //virtual void init(std::shared_ptr<Game> ig) override;

    virtual DeployList deploy(int n, bool initial=false) override; // deploy n number of troops
    virtual void attackPhase() override; // make attack moves, primary game stage
    virtual void reinforce() override;   // move troops around at the end of turn

protected:
    std::uniform_int_distribution<int> rndTerritoryDist;
};
