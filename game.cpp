#include <algorithm>
#include "game.h"

class GameOverException {
    GameOverException() {}
};

Game::Game(const Board &iBoard) : board(iBoard), state(State::Initializing),
         reinforcements(Reinforcements::Chain),
         sets(Sets::Escalating), initialDeploys(InitialDeploys::Automatic),
         dieRollDist(1, 6),
         territories(board.getTerritories()), bonusRegions(board.getBonusRegions()),
         round(0)
{
    // seed the psuedorandom number generator from a hardware random seed
    std::random_device rdev;
    rnd.seed(rdev());
}

void Game::addPlayer(std::unique_ptr<Player> iP)
{
    static int playercount = 0;
    iP->init(shared_from_this(), playercount);
    players.push_back(std::move(iP));
    pplayermap[iP.get()] = playercount; // putting raw pointer to player into map, for "authentication"
    playercount++;
}

void Game::setFog()
{
    if (state == State::Initializing)
        fog = true;
}

void Game::setTrench()
{
    if (state == State::Initializing)
        trench = true;
}

void Game::setup()
{
    nplayers = players.size();

    // initialize vectors
    boardState.resize(nplayers);
    for (auto &i : boardState)
        i = std::pair<int, int>{neutral, 1};
    extraStack.resize(territories.size());
    playerHoldings.resize(nplayers);

    // deal out territories
    std::vector<int> dealableTerritories;
    // to deal out territories we first build a list of all
    // territories in the map that should not start out neutral.
    for (int c=0; c<territories.size(); c++) {
        if (!territories[c].initNeutral)
            dealableTerritories.push_back(c);
    }
    // Then shuffle the list
    std::shuffle(dealableTerritories.begin(), dealableTerritories.end(), rnd);
    // Deal!
    int tertsPP = dealableTerritories.size() / nplayers;
    for (int c=0; c<tertsPP*nplayers ; c++)
        boardState[dealableTerritories[c]].first = c % nplayers;

    // shuffle the "extra" deck, first with just tiers then
    // with territory info added
    for (int c=0; c<extraStack.size(); c++) {
        if (c % 3 == 0)
            extraStack[c].tier = ExtraTier::First;
        if (c % 3 == 1)
            extraStack[c].tier = ExtraTier::Second;
        if (c % 3 == 2)
            extraStack[c].tier = ExtraTier::Third;
    }
    std::shuffle(extraStack.begin(), extraStack.end(), rnd);
    for (int c=0; c<extraStack.size(); c++)
        extraStack[c].territory = c;
    std::shuffle(extraStack.begin(), extraStack.end(), rnd);

    if (initialDeploys == InitialDeploys::Automatic) {
        // deploy three armies on each territory
        for (auto &i : boardState)
            i.second = 3;
        state = State::InProgress;
    } else {
        // ask players to deploy
        state = State::InitialDeployment;
        int numToDeploy = dealableTerritories.size() * 2;

        for (auto &p : players) {
            // someday maybe make each a thread to allow for asynchronous
            // deploys by human players.
            p->deploy(numToDeploy, true);
        }
    }

    // who starts?
    std::uniform_int_distribution<int> rndPlayerDist(0, players.size()-1);
    startplayer = rndPlayerDist(rnd);
}

void Game::mainLoop()
{
    turn=startplayer;
    try {
        do {
            if (turn==startplayer)
                round++;

            // auto deployments and neutral reversions

            // calculate how many troops are due

            // player deploys

            // player attacks

            // player reinforces

            turn++;
            if (turn == nplayers)
                turn=0;
        } while (true); // exit via an exception when there is a winner
    } catch (const GameOverException& e) {}
}

void Game::setLogger(std::unique_ptr<BasicLogger> bl)
{
    logger = std::move(bl);
}

const BoardView Game::getBoardView(Player *pp)
{
    if (!fog) // if not fog, we don't need anything special here
        return boardState;

    // TODO for fog
    int player = pplayermap[pp];
}


