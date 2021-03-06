#include <algorithm>
#include "game.h"

class GameOverException {
public:
    GameOverException() {}
};

Game::Game(const Board &iBoard) : board(iBoard), state(State::Initializing),
         reinforcements(Reinforcements::Chain),
         sets(Sets::Escalating), initialDeploys(InitialDeploys::Automatic),
         dieRollDist(1, 6),
         territories(board.getTerritories()), bonusRegions(board.getBonusRegions()),
         round(1), players_eliminated(0)
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

void Game::setManualDeploy()
{
    if (state == State::Initializing)
        initialDeploys = InitialDeploys::Manual;
}

void Game::setExtra(Sets iSets)
{
    if (state == State::Initializing)
        sets = iSets;
}

void Game::setReinforce(Reinforcements r)
{
    if (state == State::Initializing)
        reinforcements = r;
}

void Game::setup()
{
    nplayers = players.size();

    // initialize vectors
    boardState.resize(territories.size());
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
        state = State::InProgress;
    }

    // who starts?
    std::uniform_int_distribution<int> rndPlayerDist(0, players.size()-1);
    startplayer = rndPlayerDist(rnd);
}

void Game::mainLoop()
{
    DeployList dl;
    ReinforceList rl;
    int r_from, r_to, r_troops;  // for reinforcement
    bool right_players_and_armies, valid_reinforce_path;
    int worksum;
    turn=startplayer;
    try {
        do {
            turn_state = TurnState::Deploy;

            // auto deployments and neutral reversions
            for (int i=0; i<territories.size(); i++) {
                if (boardState[i].first == turn) {
                    int n = territories[i].autoDeploy;
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

            logger->boardState(boardState, players);

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
                        logger->troopsForBonusRegion(players[turn], br.bonusForAll, i, 0);
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

            turn_state = TurnState::Attack;

            // player attacks
            // the meat of the code here will be in a callback
            acquire_extra_flag = false;
            players[turn]->attackPhase();

            turn_state = TurnState::Reinforce;

            // player reinforces
            rl = players[turn]->reinforce();

            if (reinforcements != Reinforcements::None) {
                for (const auto &reinforce : rl) {
                    std::tie(r_from, r_to, r_troops) = reinforce;
                    right_players_and_armies = boardState[r_from].first == turn and
                            boardState[r_to].first == turn and boardState[r_from].second > r_troops;

                    if (reinforcements == Reinforcements::Adjacent) {
                        auto & can_reinforce = territories[r_from].canAttack;
                        valid_reinforce_path = std::find(can_reinforce.begin(), can_reinforce.end(), r_to) != can_reinforce.end();
                    } else {
                        // Reinforcements should be Chain or Unlimited, so any path through our territories works
                        auto valid_destinations = get_reinforce_area(turn, r_from);
                        valid_reinforce_path = std::find(valid_destinations.begin(), valid_destinations.end(), r_to) != valid_destinations.end();
                    }

                    if (valid_reinforce_path) {
                        boardState[r_from].second -= r_troops;
                        boardState[r_to].second += r_troops;
                        logger->reinforce(players[turn], r_from, r_to, r_troops);
                    }

                    if (reinforcements != Reinforcements::Unlimited)
                        break;  // if any more in the list, ignore them unless Unlimited
                }
            }

            // did they win an extra?
            if (acquire_extra_flag) {
                if (!extraStack.empty()) {
                    Extra e = extraStack.front();
                    extraStack.pop_front();
                    playerHoldings[turn].push_back(e);
                }
            }

            unsigned int current_player = turn;
            do {
                turn++;
                if (turn==startplayer) {
                    round++;
                    logger->advanceRound(round);
                }

                if (turn == nplayers)
                    turn = 0;
            } while (is_eliminated(turn));
            if (turn == current_player) {
                throw GameOverException();
            }
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
}

AttackResult Game::attack(int attackFrom, int attackTo, bool doOrDie)
{
    int& armiesOnSource = boardState[attackFrom].second;
    int& armiesOnDest = boardState[attackTo].second;

    int myDice, opponentDice;

    turn_state = TurnState::Attack;  // in case of another attack without an advance

    // ensure proper attack:
    // player owns attacking country, does NOT own attacked country,
    // and attacking country has at least 2 armies
    if (boardState[attackFrom].first != turn or boardState[attackTo].first == turn
        or armiesOnSource < 2)
        throw InvalidAttackException();
    int numLost=0, numOpponentLost=0;
    std::vector<int> myRolls, opponentRolls;
    myRolls.reserve(3);
    opponentRolls.reserve(2);
    bool stop = not doOrDie;

    do {
        myDice = std::min(armiesOnSource-1,3);
        opponentDice = std::min(armiesOnDest,2);
        // roll the dice
        // attacker rolls the lesser of one less than his source armies
        // (since one has to stay behind) or 3 (the max you can roll)
        for (int i=0; i<myDice; i++)
            myRolls.push_back(dieRoll());
        std::sort(std::begin(myRolls), std::end(myRolls), std::greater<int>());
        // attackee rolls the lesser of his defending armies or 2
        for (int i=0; i<opponentDice; i++)
            opponentRolls.push_back(dieRoll());
        std::sort(std::begin(opponentRolls), std::end(opponentRolls));
        
        for (int i=0; i<std::min(myDice, opponentDice); i++)
            if (opponentRolls[i] >= myRolls[i]) {
                numLost++;
                armiesOnSource--;
            }
            else {
                numOpponentLost++;
                armiesOnDest--;
            }

        if (armiesOnDest != 0) {

            if (doOrDie) {  // do we stop yet?
                if (armiesOnSource < 3)
                    stop = true;
            }

        } else {
            // Attacker won!
            logger->attack(players[turn], attackFrom, attackTo, true, numLost, numOpponentLost);
            acquire_extra_flag = true;
            // advance one, change owner
            int old_owner = boardState[attackTo].first;
            boardState[attackTo].first = turn;
            armiesOnDest = 1;
            armiesOnSource--;
            turn_state = TurnState::Advance;
            advance_from_to = std::make_pair(attackFrom, attackTo);

            if (old_owner != -1) {
                int player_has = std::count_if(boardState.begin(), boardState.end(),
                                               [old_owner](std::pair<int, int> t) { return t.first == old_owner; });
                if (player_has == 0) {
                    player_out(old_owner);
                    logger->eliminated(players[old_owner]);
                }
            }

            return std::make_tuple(numLost, numOpponentLost, true, armiesOnSource);
        }
    } while (!stop);

    logger->attack(players[turn], attackFrom, attackTo, false, numLost, numOpponentLost);
    return std::make_tuple(numLost, numOpponentLost, false, armiesOnSource);
}

void Game::advance(int armies) {
    if (armies < boardState[advance_from_to.first].second) { // at least one must remain in the source territory
        boardState[advance_from_to.first].second -= armies;
        boardState[advance_from_to.second].second += armies;
        turn_state = TurnState::Attack;
        logger->advance(players[turn], advance_from_to.first, advance_from_to.second, armies);
    }
}

std::vector<int> Game::get_reinforce_area(int player, int from) {
    std::vector<int> list{from};  // building list of places we can go
    std::vector<bool> checked{false};  // corresponding to above; whether we have checked those for connections yet
    int tert = from;
    int index = 0;  // index into these vectors we're currently looking at

    do {
        // add any territories we haven't seen yet to list, and false to checked
        for (int t : territories[tert].canAttack) {
            if (boardState[t].first == player and std::find(list.begin(), list.end(), t) == list.end()) {
                list.push_back(t);
                checked.push_back(false);
            }
        }

        checked[index] = true;

        // next territory to check, if any
        auto it = std::find(checked.begin(), checked.end(), false);
        if (it == checked.end())
            return list;
        index = std::distance(checked.begin(), it);
        tert = list[index];
    } while (true);
}

int Game::extra_escalating_next()
{
    static int current = 2;
    if (current < 15)
        current += 2;
    else
        current += 5;

    return current;
}

