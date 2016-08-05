#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

using namespace std;

class Board
{
    struct TerritoryInfo {
        string name; // short name or "ID"
        string fullName; // display name
        vector<int> canAttack; // territories we can attack from here
        int autoDeploy=0; // number troops auto-deployed if a player holds at beginning of turn
        int revertNeutral=0; // number neutral armies if player holds at beginning of turn 0=no
    };
    struct BonusRegionInfo {
        string name;
        string fullName;
        int bonusForAll=0; // bonus given if all territories held
        vector<int> heldBonus; // bonus given if [x] territories held
        vector<int> territories; // territories in our bonus region
    };

public:
    Board(const string& filename);

protected:
    vector<TerritoryInfo> territories;
    vector<BonusRegionInfo> bonusRegions;
    unordered_map<string, int> territoriesById;
    unordered_map<string, int> bonusRegionsById;

    int armyPerTerritories=3; // Player gets one army for this number of territories held
    int minArmiesToDeploy=3; // Minimum a player will get on any turn

public:
    inline const vector<TerritoryInfo>& getTerritories() { return territories; }
    inline const vector<BonusRegionInfo>& getBonusRegions() { return bonusRegions; }
    inline int getTerritoryById(string& id) { return territoriesById.at(id); }

};

// Exception for YAML parse issues
class MapConfigParseException : public runtime_error {
public:
    MapConfigParseException(const string& msg) : runtime_error(msg) {}
};
