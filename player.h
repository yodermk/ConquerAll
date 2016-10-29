#pragma once
#include <memory>

class Game;

using namespace std;

class Player
{
public:
    Player();
    void init(shared_ptr<Game> ig) { g = ig; }

    // Actions a player needs to do
    virtual void deploy(int n) =0; // deploy n number of troops
    virtual void attackPhase() =0; // make attack moves, primary game stage
    virtual void reinforce() =0;   // move troops around at the end of turn

private:
    shared_ptr<Game> g;
};

