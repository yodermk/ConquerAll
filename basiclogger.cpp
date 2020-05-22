#include <iomanip>
#include "basiclogger.h"

BasicLogger::BasicLogger(std::ostream &iOut, const Board& iBoard)
    : out(iOut), board(iBoard)
{

}

void BasicLogger::deploy(const std::shared_ptr<Player> &p, int territory, int num)
{
    out << p->getName() << " deployed " << num << " armies on "
        << board.getTerritories()[territory].getName() << std::endl << std::flush;
}

void BasicLogger::autoDeploy(const std::shared_ptr<Player> &p, int territory, int num)
{
    out << num << " armies were added to "
        << board.getTerritories()[territory].getName() << std::endl << std::flush;
}

void BasicLogger::revertNeutral(const std::shared_ptr<Player> &p, int territory, int num)
{
    out << board.getTerritories()[territory].getName() << " has reverted to "
        << num << " neutral troops." << std::endl << std::flush;
}

void BasicLogger::troopsForTerritories(const std::shared_ptr<Player> &p, int troops, int territories)
{
    out << p->getName() << " received " << troops << " troops for "
        << territories << " territories." << std::endl << std::flush;
}

void BasicLogger::troopsForBonusRegion(const std::shared_ptr<Player> &p, int troops, int bonusRegion, int part)
{
    out << p->getName() << " received " << troops << " troops for holding ";
    if (part>0)
        out << part << " territories of ";
    out << board.getBonusRegions()[bonusRegion].getName() << std::endl << std::flush;

}

void BasicLogger::attack(const std::shared_ptr<Player> &p, int fromTerritory, int toTerritory, bool success, int numLost,
                         int numOpponentLost) {
    out << p->getName() << " is attacking " << board.getTerritories()[toTerritory].getName() << " from " <<
        board.getTerritories()[fromTerritory].getName() << std::endl << std::flush;
    if (success)
        out << "Succeeded!";
    else
        out << "Failed.";

    out << " Lost " << numLost << " armies to opponent's loss of " << numOpponentLost << std::endl << std::flush;
}

void BasicLogger::advance(const std::shared_ptr<Player> &p, int fromTerritory, int toTerritory, int armies) {
    out << p->getName() << " advanced " << armies << " from " << board.getTerritories()[fromTerritory].getName() << " to " <<
        board.getTerritories()[toTerritory].getName() << std::endl << std::flush;
}

void BasicLogger::eliminated(const std::shared_ptr<Player> &p) {
    out << p->getName() << " has been eliminated!" << std::endl << std::flush;
}

void BasicLogger::boardState(const BoardView &state, const Board &b) {
    auto &ts = b.getTerritories();
    for (int t=0; t<ts.size(); t++) {
        out << std::setw(20) << ts[t].getName() << " : " << state[t].first << ":" << state[t].second << std::endl;
    }
    out << std::flush;
}
