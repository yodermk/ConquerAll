#include <iostream>
#include <memory>
#include "board.h"
#include "game.h"
#include "randomaiplayer.h"
#include "basiclogger.h"

void test() {
    std::shared_ptr<Game> g = nullptr;
    try {
        Board b("maps/texas.yml");
        g = std::make_shared<Game>(b);

        g->addPlayer(std::make_unique<RandomAIPlayer>("Bob"));
        g->addPlayer(std::make_unique<RandomAIPlayer>("Bert"));
        g->addPlayer(std::make_unique<RandomAIPlayer>("Harry"));
        g->addPlayer(std::make_unique<RandomAIPlayer>("George"));

        g->setLogger(std::make_unique<BasicLogger>(std::cout, g->getBoard()));
        g->setup();
        g->mainLoop();

    } catch (const MapConfigParseException& e) {
        std::cout << "YAML Exception: " << e.what() << std::endl;
        return;
    }

}

int main(int argc, char *argv[])
{
    test();
    return 0;
}
