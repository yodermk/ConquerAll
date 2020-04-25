#include <algorithm>
#include "player.h"
#include "game.h"
#include "board.h"

Player::Player(std::string iName) : name(iName)
{
}

std::vector<int> Player::myTerritories()
{
    std::vector<int> terts;
    BoardView bv = g->getBoardView(this);
    for (int i=0; i<bv.size(); i++)
        if (bv[i].first == me)
            terts.push_back(i);
    return terts;
}

PossibleAttacks Player::getPossibleAttacks(Player::SortAttacks sort_by)
{
    BoardView bv = g->getBoardView(this);
    PossibleAttacks r;
    auto terts = g->board.getTerritories();

    for (int source=0; source<bv.size(); source++)
        if (bv[source].first == me)
            for (int target : terts[source].canAttack)
                if (bv[target].first != me)
                    r.push_back({source, bv[source].second, target, bv[target].second});

    switch (sort_by) {
        case SortAttacks::AttackingArmies:
            std::sort(r.begin(), r.end(), [](PossibleAttack a, PossibleAttack b) {return std::get<1>(a) < std::get<1>(b);} );
            break;
        case SortAttacks::DefendingArmies:
            std::sort(r.begin(), r.end(), [](PossibleAttack a, PossibleAttack b) {return std::get<3>(a) < std::get<3>(b);} );
            break;
        case SortAttacks::BestOdds:
            std::sort(r.begin(), r.end(), [](PossibleAttack a, PossibleAttack b) {return std::get<1>(a)/std::get<3>(a) < std::get<1>(b)/std::get<3>(b);} );
            break;
    }

    return r;
}

