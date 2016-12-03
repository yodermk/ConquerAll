#include <iostream>
#include <memory>
#include "board.h"
#include "game.h"
#include "basicaiplayer.h"
#include "basiclogger.h"

void test() {
    std::shared_ptr<Game> g = nullptr;
    try {
        Board b("maps/texas.yml");
        g = std::make_shared<Game>(b);
    } catch (const MapConfigParseException& e) {
        std::cout << "YAML Exception: " << e.what() << std::endl;
        return;
    }

    g->addPlayer(std::make_unique<BasicAIPlayer>("Bob"));
    g->addPlayer(std::make_unique<BasicAIPlayer>("Bert"));
    g->addPlayer(std::make_unique<BasicAIPlayer>("Harry"));
    g->addPlayer(std::make_unique<BasicAIPlayer>("George"));

    g->setLogger(std::make_unique<BasicLogger>(std::cout, g->getBoard()));
    g->setup();

}

int main(int argc, char *argv[])
{
    test();
    return 0;
}
