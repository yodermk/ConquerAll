#pragma once

#include <ostream>
#include <memory>
#include "board.h"
#include "player.h"

class BasicLogger
{
public:
    BasicLogger(std::ostream& iOut, const Board& iBoard);
    virtual void deploy(const std::shared_ptr<Player> p, int territory, int num);
    virtual void autoDeploy(const std::shared_ptr<Player> p, int territory, int num);
    virtual void revertNeutral(const std::shared_ptr<Player> p, int territory, int num);
    virtual void troopsForTerritories(const std::shared_ptr<Player> p, int troops, int territories);
    virtual void troopsForBonusRegion(const std::shared_ptr<Player> p, int troops, int bonusRegion, int part=0);
    virtual ~BasicLogger();

protected:
    std::ostream& out;
    const Board& board;
};
