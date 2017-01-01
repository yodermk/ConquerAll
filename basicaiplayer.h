#pragma once
#include <random>
#include "player.h"

class BasicAIPlayer : public Player
{
public:
    BasicAIPlayer(std::string iName);

    virtual DeployList deploy(int n, bool initial=false) override; // deploy n number of troops
    virtual void attackPhase() override; // make attack moves, primary game stage
    virtual ReinforceList reinforce() override;   // move troops around at the end of turn

protected:
    std::mt19937 rnd;
};

