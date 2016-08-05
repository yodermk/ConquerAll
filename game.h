#pragma once
#include "board.h"
#include "player.h"

class Game
{
public:
    Game(const Board& iBoard);
    void addPlayer(Player iP);
    void setFog();
    void setTrench();

    enum class Reinforcements { None, Adjacent, Chain, Unlimited };
    enum class Sets { Fixed, Escalating, Nuclear, Zombie };
    enum class State { Initializing, InitialDeployment, InProgress, Finished };

protected:
    const Board& board;
    vector<Player> players;

    Reinforcements reinforcements;
    Sets sets;
    State state;
    bool fog = false;
    bool trench = false;
};
