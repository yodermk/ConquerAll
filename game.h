#pragma once
#include <memory>
#include <random>
#include <map>
#include "board.h"
#include "player.h"
#include "basiclogger.h"

// type for returning a view of the board to a player
// For every territory: <Player #, Armies>
using BoardView = std::vector<std::pair<int, int>>;

// type for returning the result of an attack.
// <troops you lost, troops opponent lost, territory conquered>
using AttackResult = std::tuple<int, int, bool>;

// The main Game object; contains info about the state of the game
// and logic to control it.

class Game : public std::enable_shared_from_this<Game>
{
public:
    enum class Reinforcements { None, Adjacent, Chain, Unlimited };
    enum class Sets { Fixed, Escalating, Nuclear, Zombie };
    enum class InitialDeploys { Automatic, Manual };
    enum class State { Initializing, InitialDeployment, InProgress, Finished };
    enum class ExtraTier { First, Second, Third };

    Game(const Board& iBoard);
    void addPlayer(std::unique_ptr<Player> iP);
    void setFog();
    void setTrench();
    void setup();
    void mainLoop();
    void setLogger(std::unique_ptr<BasicLogger> bl);
    inline const Board& getBoard() { return board; }
    const BoardView getBoardView(Player *pp);
    AttackResult attack(Player *pp, int attackFrom, int attackTo, bool doOrDie=false);

    // represents the "card" you get if you conquer at least one territory
    struct Extra {
        int territory;
        ExtraTier tier;
    };

    const int neutral = -1;  // signal that no player owns a territory
    const int shrouded = -2; // if a player can not see a location due to fog

    const Board& board;
    std::uniform_int_distribution<int> dieRollDist; // for dice rolls

protected:
    inline int dieRoll() { return dieRollDist(rnd); }

    // current game options and settings
    std::vector<std::shared_ptr<Player>> players;
    int nplayers; // number of players
    Reinforcements reinforcements;
    Sets sets;
    InitialDeploys initialDeploys;
    bool fog = false;
    bool trench = false;
    std::vector<Board::TerritoryInfo> territories;
    std::vector<Board::BonusRegionInfo> bonusRegions;
    std::unique_ptr<BasicLogger> logger;
    std::map<Player*, int> pplayermap; // used to "authenticate" requests from players, via pointer

    // For game logic
    State state;
    std::mt19937 rnd; // random number generator
    int startplayer;
    int round=0; // round number of the game (one round=each player taking his/her/its turn)
    int turn; // player who's turn it is, index into 'players'
    BoardView boardState; // the "real" board state, before modification by player views for fog
    std::vector<Extra> extraStack; // main stack of "cards"
    std::vector<std::vector<Extra>> playerHoldings; // which "cards" playe
};

class InvalidAttackException : public std::exception {
public:
    InvalidAttackException() {}
};
