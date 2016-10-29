#pragma once
#include "player.h"

class BasicAIPlayer : public Player
{
public:
    BasicAIPlayer();

    virtual void deploy(int n); // deploy n number of troops
    virtual void attackPhase(); // make attack moves, primary game stage
    virtual void reinforce();   // move troops around at the end of turn
};

