#pragma once
#include <map>
#include <memory>
#include <random>
#include <deque>
#include "board.h"
#include "player.h"
#include "basiclogger.h"

// type for returning the result of an attack.
// <troops you lost, troops opponent lost, territory conquered, remaining armies on source>
using AttackResult = std::tuple<int, int, bool, int>;

// The main Game object; contains info about the state of the game
// and logic to control it.

class Game : public std::enable_shared_from_this<Game>
{
public:
    enum class Reinforcements { None, Adjacent, Chain, Unlimited };
    enum class Sets { Fixed, Escalating, Nuclear, Zombie };
    enum class InitialDeploys { Automatic, Manual };
    enum class State { Initializing, InitialDeployment, InProgress, Finished };
    enum class TurnState { Deploy, Attack, Advance, Reinforce };
    enum class ExtraTier { First, Second, Third };

    Game(const Board& iBoard);
    void addPlayer(std::unique_ptr<Player> iP);
    void setFog();
    void setTrench();
    void setManualDeploy();
    void setExtra(Sets iSets);
    void setReinforce(Reinforcements r);
    void setup();
    void mainLoop();
    void setLogger(std::unique_ptr<BasicLogger> bl);
    inline const Board& getBoard() { return board; }
    const BoardView getBoardView(Player *pp);
    AttackResult attack(int attackFrom, int attackTo, bool doOrDie = false);
    void advance(int armies);
    bool is_eliminated(unsigned int player) const {return players_eliminated & (1u << player);}

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
    virtual int extra_escalating_next();

    // current game options and settings
    std::vector<std::shared_ptr<Player>> players;
    int nplayers; // number of players
    Reinforcements reinforcements;
    Sets sets;
    InitialDeploys initialDeploys;
    bool fog = false;
    bool trench = false;
    const std::vector<Board::TerritoryInfo> & territories;
    std::vector<Board::BonusRegionInfo> bonusRegions;
    std::unique_ptr<BasicLogger> logger;

    // For game logic
    State state;
    TurnState turn_state;
    std::pair<int,int> advance_from_to;  // After a win, note territories where the advance should be from and to, meaningful only in TurnState::Advance
    std::mt19937 rnd; // random number generator
    int startplayer;
    int round=0; // round number of the game (one round=each player taking his/her/its turn)
    int turn; // player who's turn it is, index into 'players'
    BoardView boardState; // the "real" board state, before modification by player views for fog
    std::deque<Extra> extraStack; // main stack of "cards"
    std::vector<std::vector<Extra>> playerHoldings; // which "cards" player holds
    bool acquire_extra_flag;  // set when one territory on a turn has been conquered, to signal drawing a card
    unsigned long int players_eliminated;  // bit field, each bit is true when corresponding player is out. Yes this limits us to 64 players.
    std::vector<int> get_reinforce_area(int player, int from);

    void player_out(unsigned int player) {players_eliminated |= (1u << player);}
};

class InvalidAttackException : public std::exception {
public:
    InvalidAttackException() = default;
};
