#pragma once
#include <memory>
#include <string>
#include <list>
#include <tuple>
#include <vector>

// when deploying, this will indicate a list of <territory, troops>
using DeployList = std::list<std::pair<int, int>>;
// when reinforcing, this list will be <from_tert, to_tert, troops>
using ReinforceList = std::list<std::tuple<int, int, int>>;
// list of possible attacks (from territory, # armies, to territory, # armies)
using PossibleAttack = std::tuple<int, int, int, int>;
using PossibleAttacks = std::vector<PossibleAttack>;

class Game;

class Player
{
public:
    Player(std::string iName);
    virtual void init(std::shared_ptr<Game> ig, int iMe) { g = ig; me = iMe; }

    // Actions a player needs to do
    virtual DeployList deploy(int n, bool initial=false) =0; // deploy n number of troops
    virtual void attackPhase() =0; // make attack moves, primary game stage
    virtual ReinforceList reinforce() =0;   // move troops around at the end of turn

    inline std::string getName() const { return name; }

protected:
    std::shared_ptr<Game> g;
    std::string name;
    int me;
    enum class SortAttacks { AttackingArmies, DefendingArmies, BestOdds };

    std::vector<int> myTerritories();
    PossibleAttacks getPossibleAttacks(SortAttacks sort_by);
};

