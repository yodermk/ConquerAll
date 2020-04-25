#include "board.h"
#include <yaml-cpp/yaml.h>

Board::Board(const std::string &filename)
{
    YAML::Node mapDef = YAML::LoadFile(filename);
    YAML::Node iTerts = mapDef["territories"];
    YAML::Node iRegs = mapDef["regions"];
    YAML::Node iOpts = mapDef["options"];
    size_t i;

    if (!mapDef.IsMap())
        throw MapConfigParseException("Top level YAML must be a dictionary.");
    if (!iTerts.IsSequence())
        throw MapConfigParseException("Top level \"territories\" must be a list.");
    if (!iRegs.IsSequence())
        throw MapConfigParseException("Top level \"regions\" must be a list.");

    // Optimize the vectors
    territories.reserve(iTerts.size());
    bonusRegions.reserve(iRegs.size());

    // First pass through to build "by ID" maps
    for (i=0; i<iTerts.size(); i++) {
        territoriesById[iTerts[i]["name"].as<std::string>()] = i;
    }
    for (i=0; i<iRegs.size(); i++) {
        bonusRegionsById[iRegs[i]["name"].as<std::string>()] = i;
    }

    // build territory info
    for (i=0; i<iTerts.size(); i++) {
        TerritoryInfo t;
        t.name = iTerts[i]["name"].as<std::string>();
        if (t.name.length()==0)
            throw MapConfigParseException("A territory needs a value for 'name'.");
        if (iTerts[i]["fullName"]) {
            t.fullName = iTerts[i]["fullName"].as<std::string>();
            if (t.fullName.length()==0)
                t.fullName = t.name;
        }
        if (iTerts[i]["autodeploy"]) {
            int ad = iTerts[i]["autodeploy"].as<int>();
            if (ad > 0)
                t.autoDeploy = ad;
        }
        if (iTerts[i]["initNeutral"]) {
            int in = iTerts[i]["initNeutral"].as<int>();
            if (in > 0)
                t.autoDeploy = in;
        }
        if (iTerts[i]["revertNeutral"]) {
            int rn = iTerts[i]["revertNeutral"].as<int>();
            if (rn > 0)
                t.revertNeutral = rn;
        }
        YAML::Node iCanAtck = iTerts[i]["canAttack"];
        if (!iCanAtck.IsSequence())
            throw MapConfigParseException("Territory " + t.name + " needs a canAttack array.");
        for (size_t j=0; j<iCanAtck.size(); j++) {
            std::string n = iCanAtck[j].as<std::string>();
            if (territoriesById.find(n) == territoriesById.end())
                throw MapConfigParseException(n + " is referenced in canAttack of " + t.name + " but is not the ID of a territory.");
            t.canAttack.push_back(territoriesById.at(n));
        }

        territories.push_back(t);
    }

    // build bonus region info
    for (i=0; i<iRegs.size(); i++) {
        BonusRegionInfo b;
        b.name = iRegs[i]["name"].as<std::string>();
        if (b.name.length()==0)
            throw MapConfigParseException("A bonus region needs a value for 'name'.");
        b.fullName = iRegs[i]["fullName"].as<std::string>();
        if (b.fullName.length()==0)
            b.fullName = b.name;
        if (iRegs[i]["bonusForAll"]) {
            int bfa = iRegs[i]["bonusForAll"].as<int>();
            if (bfa > 0)
                b.bonusForAll = bfa;
        }
        YAML::Node iTerts = iRegs[i]["territories"];
        for (size_t j=0; j<iTerts.size(); j++) {
            std::string n = iTerts[j].as<std::string>();
            if (territoriesById.find(n) == territoriesById.end())
                throw MapConfigParseException(n + " is referenced in 'territories' of " + b.name + " but is not the ID of a territory.");
            b.territories.push_back(territoriesById.at(n));
        }

        bonusRegions.push_back(b);
    }

    // Check for options
    if (!iOpts.IsMap())
        return;
    if(iOpts["armyPerTerritories"]) {
        int apt = iOpts["armyPerTerritories"].as<int>();
        if (apt > 0)
            armyPerTerritories = apt;
    }
    if (iOpts["minArmiesToDeploy"]) {
        int atd = iOpts["minArmiesToDeploy"].as<int>();
        if (atd > 0)
            minArmiesToDeploy = atd;
    }

}

int Board::getArmyPerTerritories() const
{
    return armyPerTerritories;
}

int Board::getMinArmiesToDeploy() const
{
    return minArmiesToDeploy;
}

