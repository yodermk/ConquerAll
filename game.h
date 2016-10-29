#pragma once
#include <memory>
#include "board.h"
#include "player.h"

// The main Game object; contains info about the state of the game
// and logic to control it.

class Game
{
public:
    Game(const Board& iBoard);
    void addPlayer(std::unique_ptr<Player> iP);
    void setFog();
    void setTrench();
    void start();

    enum class Reinforcements { None, Adjacent, Chain, Unlimited };
    enum class Sets { Fixed, Escalating, Nuclear, Zombie };
    enum class InitialDeploys { Automatic, Manual };
    enum class State { Initializing, InitialDeployment, InProgress, Finished };

    const int neutral = -1; // signal that no player owns a territory

protected:
    // current game options and settings
    const Board& board;
    vector<std::shared_ptr<Player>> players;
    Reinforcements reinforcements;
    Sets sets;
    InitialDeploys initialDeploys;
    bool fog = false;
    bool trench = false;

    // For game logic
    State state;
    int turn; // player who's turn it is, index into 'players'
    vector<int> owner; // player who owns each territory on the Board
                       // Key is index into territories array.
                       // Value is index into players array, -1 = neutral (^ const defined)
    vector<int> armies; // How many armies are on a territory
                        // Key is index into territories array
};
