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
    std::uniform_int_distribution<int> rndt(0, terts.size()-1);
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

        // std::vector<PossibleAttack> attack;
        // std::sample(possible_attacks.begin(), possible_attacks.end(), attack.begin(), 1, rnd);  Segfaults, why?
        std::uniform_int_distribution<int> rnda(0, possible_attacks.size()-1);
        int attack_idx = rnda(rnd);

        int iLost, opponentLost, armiesLeft;
        bool result;
        try {
            std::tie(iLost, opponentLost, result, armiesLeft) = g->attack(std::get<0>(possible_attacks[attack_idx]),
                                                                          std::get<2>(possible_attacks[attack_idx]),
                                                                          true);
        } catch (InvalidAttackException &e) {
            return;
        }
        if (result) {  // if attack succeeded, we need to advance some
            std::uniform_int_distribution<int> rndi(0, armiesLeft);  // yes, zero is valid as is remaining armies minus 1
            int advance_armies = rndi(rnd);
            g->advance(advance_armies);
        }
    } while (true);
}

ReinforceList RandomAIPlayer::reinforce()
{
    ReinforceList rl;
    return rl;
}
