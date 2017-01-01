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
}

void RandomAIPlayer::attackPhase()
{

}

ReinforceList RandomAIPlayer::reinforce()
{
    // return default empty list so no reinforce
}
