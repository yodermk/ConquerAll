#pragma once

#include <ostream>
#include "board.h"
#include "player.h"

class BasicLogger
{
public:
    BasicLogger(std::ostream& iOut, const Board& iBoard);
    virtual void deploy(const Player& p, int territory, int num);
    virtual ~BasicLogger();

protected:
    std::ostream& out;
    const Board& board;
};
