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
    DeployList dl;
    ReinforceList rl;
    int worksum;
    turn=startplayer;
    try {
        do {
            if (turn==startplayer)
                round++;

            // auto deployments and neutral reversions
            for (int i=0; i<territories.size(); i++) {
                if (boardState[i].first == turn) {
                    register int n = board.getTerritories()[i].autoDeploy;
                    if (n) {
                        boardState[i].second += n;
                        logger->autoDeploy(players[turn], i, n);
                    }
                    n = board.getTerritories()[i].revertNeutral;
                    if (n) {
                        boardState[i].first = neutral;
                        boardState[i].second = n;
                        logger->revertNeutral(players[turn], i, n);
                    }
                }
            }

            // calculate how many troops are due
            // start by counting the territories the player has
            int c = std::count_if(std::cbegin(boardState), std::cend(boardState),
                                  [turn=turn](std::pair<int,int> p){return turn==p.first;} );
            // turn that into armies for territories as specified by map, taking the minimum into account
            int t = c / board.getArmyPerTerritories();
            t = std::max(t, board.getMinArmiesToDeploy());
            // add any bonus region bonuses
            auto brs = board.getBonusRegions();
            for (int i=0; i<brs.size(); i++) {
                auto br = brs[i];
                int iHave=0;
                for (const int &t : br.territories)
                    if (boardState[t].first == turn)
                        iHave++;
                bool iHaveAll = iHave == br.territories.size();
                if (br.bonusForAll) {
                    // bonus region has one bonus for holding all its territories
                    if (iHaveAll) {
                        t += br.bonusForAll;
                        logger->troopsForBonusRegion(players[turn], br.bonusForAll, i);
                    }
                } else {
                    // bonus region has variable bonus depending on how many regions held
                    int bonus = br.heldBonus[iHave];
                    t += bonus;
                    if (bonus>0)
                        logger->troopsForBonusRegion(players[turn], bonus, i, iHave);
                }
            }

            // player deploys
            dl = players[turn]->deploy(t);
            // DeployList is a list of <territory, #troops> pairs
            // First we just make sure it's not trying to drop too many. Really the Player
            // subclasses are expected to get this right, but if they try to drop too many,
            // we won't deploy anything; if they drop too few, we'll just follow instructions.
            worksum = 0;
            for (const auto &d : dl)
                worksum += d.second;
            if (worksum <= t) {
                // now do the drops
                for (const auto &d : dl) {
                    boardState[d.first].second += d.second;
                    logger->deploy(players[turn], d.first, d.second);
                }
            }

            // player attacks
            // the meat of the code here will be in a callback
            players[turn]->attackPhase();

            // player reinforces
            rl = players[turn]->reinforce();

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

AttackResult Game::attack(Player *pp, int attackFrom, int attackTo, bool doOrDie)
{
    int& armiesOnSource = boardState[attackFrom].second;
    int& armiesOnDest = boardState[attackTo].second;
    int player = pplayermap[pp];

    // ensure proper attack:
    // player owns attacking country, does NOT own attacked country,
    // and attacking country has at least 2 armies
    if (boardState[attackFrom].first != player or boardState[attackTo].first == player
        or armiesOnSource < 2)
        throw InvalidAttackException();
    int numLost=0, numOpponentLost=0;
    std::vector<int> myRolls, opponentRolls;
    myRolls.reserve(3);
    opponentRolls.reserve(2);

    do {
        // roll the dice
        // attacker rolls the lesser of one less than his source armies
        // (since one has to stay behind) or 3 (the max you can roll)
        for (int i=0; i<std::min(armiesOnSource-1,3); i++)
            myRolls.push_back(dieRoll());
        std::sort(std::begin(myRolls), std::end(myRolls));
        // attackee rolls the lesser of his defending armies or 2
        for (int i=0; i<std::min(armiesOnDest,2); i++)
            opponentRolls.push_back(dieRoll());
        std::sort(std::begin(opponentRolls), std::end(opponentRolls));

    } while (!doOrDie);

    return std::make_tuple(numLost, numOpponentLost, false );
}


