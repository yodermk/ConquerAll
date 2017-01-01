#include "basicaiplayer.h"

BasicAIPlayer::BasicAIPlayer(std::string iName) : Player(iName)
{
    // seed the psuedorandom number generator from a hardware random seed
    std::random_device rdev;
    rnd.seed(rdev());
}

DeployList BasicAIPlayer::deploy(int n, bool initial)
{

}

void BasicAIPlayer::attackPhase()
{

}

ReinforceList BasicAIPlayer::reinforce()
{

}
