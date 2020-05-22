#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

// type for returning a view of the board to a player
// For every territory: <Player #, Armies>
using BoardView = std::vector<std::pair<int, int>>;

// A Board object holds the info from the corresponding map file.
// It contains static information about the map in use.

class Board
{
public:
    struct TerritoryInfo {
        std::string name; // short name or "ID"
        std::string fullName; // display name
        std::vector<int> canAttack; // territories we can attack from here
        int autoDeploy=0; // number troops auto-deployed if a player holds at beginning of turn
        int initNeutral=0; // will this start neutral, and with how many armies?
        int revertNeutral=0; // number neutral armies if player holds at beginning of turn 0=no
        std::string getName() const {if (fullName.empty()) return name; else return fullName;}
    };
    struct BonusRegionInfo {
        std::string name;
        std::string fullName;
        int bonusForAll=0; // bonus given if all territories held
        std::vector<int> heldBonus; // bonus given if [x] territories held
        std::vector<int> territories; // territories in our bonus region
        std::string getName() const {if (fullName.empty()) return name; else return fullName;}
    };

    Board(const std::string& filename);

protected:
    std::vector<TerritoryInfo> territories;
    std::vector<BonusRegionInfo> bonusRegions;
    std::unordered_map<std::string, int> territoriesById;
    std::unordered_map<std::string, int> bonusRegionsById;

    int armyPerTerritories=3; // Player gets one army for this number of territories held
    int minArmiesToDeploy=3; // Minimum a player will get on any turn

public:
    inline const std::vector<TerritoryInfo> & getTerritories() const { return territories; }
    inline const std::vector<BonusRegionInfo> & getBonusRegions() const { return bonusRegions; }
    inline int getTerritoryById(std::string& id) { return territoriesById.at(id); }
    int getMinArmiesToDeploy() const;
    int getArmyPerTerritories() const;
};

// Exception for YAML parse issues
class MapConfigParseException : public std::runtime_error {
public:
    MapConfigParseException(const std::string& msg) : std::runtime_error(msg) {}
};
