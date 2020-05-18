#include "basiclogger.h"

BasicLogger::BasicLogger(std::ostream &iOut, const Board& iBoard)
    : out(iOut), board(iBoard)
{

}

void BasicLogger::deploy(const std::shared_ptr<Player> &p, int territory, int num)
{
    out << p->getName() << " deployed " << num << " armies on "
        << board.getTerritories()[territory].fullName << std::endl;
}

void BasicLogger::autoDeploy(const std::shared_ptr<Player> &p, int territory, int num)
{
    out << num << " armies were added to "
        << board.getTerritories()[territory].fullName << std::endl;
}

void BasicLogger::revertNeutral(const std::shared_ptr<Player> &p, int territory, int num)
{
    out << board.getTerritories()[territory].fullName << " has reverted to "
        << num << " neutral troops." << std::endl;
}

void BasicLogger::troopsForTerritories(const std::shared_ptr<Player> &p, int troops, int territories)
{
    out << p->getName() << " received " << troops << " troops for "
        << territories << " territories." << std::endl;
}

void BasicLogger::troopsForBonusRegion(const std::shared_ptr<Player> &p, int troops, int bonusRegion, int part)
{
    out << p->getName() << " received " << troops << " troops for holding ";
    if (part>0)
        out << part << " territories of ";
    out << board.getBonusRegions()[bonusRegion].fullName << std::endl;

}

void BasicLogger::attack(const std::shared_ptr<Player> &p, int fromTerritory, int toTerritory, bool success, int numLost,
                         int numOpponentLost) {
    out << p->getName() << " is attacking " << board.getTerritories()[toTerritory].fullName << " from " <<
        board.getTerritories()[fromTerritory].fullName << std::endl;
    if (success)
        out << "Succeeded!";
    else
        out << "Failed.";

    out << " Lost " << numLost << " armies to opponent's loss of " << numOpponentLost << std::endl;
}

void BasicLogger::advance(const std::shared_ptr<Player> &p, int fromTerritory, int toTerritory, int armies) {
    out << p->getName() << " advanced " << armies << " from " << board.getTerritories()[fromTerritory].fullName << " to " <<
        board.getTerritories()[toTerritory].fullName << std::endl;
}
