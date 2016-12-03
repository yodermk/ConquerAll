#include "basiclogger.h"

BasicLogger::BasicLogger(std::ostream &iOut, const Board& iBoard)
    : out(iOut), board(iBoard)
{

}

void BasicLogger::deploy(const Player &p, int territory, int num)
{
    out << p.getName() << " deployed " << num << " armies on "
        << board.getTerritories()[territory].fullName << std::endl;
}

BasicLogger::~BasicLogger()
{

}
