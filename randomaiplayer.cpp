#include <algorithm>
#include <tuple>
#include "randomaiplayer.h"
#include "game.h"

RandomAIPlayer::RandomAIPlayer(std::string iName) : BasicAIPlayer(iName)
{

}

//void RandomAIPlayer::init(std::shared_ptr<Game> ig)
//{
//    BasicAIPlayer::init(ig);
    //rndTerritoryDist(0, g->getBoard().territories.size());
//}

DeployList RandomAIPlayer::deploy(int n, bool initial)
{
    DeployList result;
    std::vector<int> terts = myTerritories();
    std::uniform_int_distribution<int> rndt(0, terts.size());
    for (int i=0; i<n; i++) {
        int which = rndt(rnd);
        result.push_back(std::make_pair(terts[which], 1));
    }
    
    return result;
}

void RandomAIPlayer::attackPhase()
{
    do {
        auto possible_attacks = getPossibleAttacks(SortAttacks::BestOdds);
        if (possible_attacks.empty())
            return;

        std::vector<PossibleAttack> attack;
        auto do_attack = std::sample(possible_attacks.begin(), possible_attacks.end(), attack.begin(), 1, rnd);

        AttackResult result = g->attack(std::get<0>(do_attack[0]), std::get<2>(do_attack[0]), true);
        if (std::get<2>(result)) {  // if attack succeeded, we need to advance some

        }
    } while (true);
}

ReinforceList RandomAIPlayer::reinforce()
{
    // return default empty list so no reinforce
}
