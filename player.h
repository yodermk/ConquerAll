#pragma once
#include <memory>

class Game;

using namespace std;

class Player
{
public:
    Player();
    void init();

    // Actions a player needs to do
    void deploy(int n); // deploy n number of troops

private:
    shared_ptr<Game> g;
};

