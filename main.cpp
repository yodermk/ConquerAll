#include <iostream>
#include <memory>
#include "board.h"
#include "game.h"

using namespace std;

void test() {
    try {
        Board b("maps/texas.yml");
        auto g = make_shared<Game>(b);
    } catch (const MapConfigParseException& e) {
        cout << "YAML Exception: " << e.what() << endl;
        return;
    }

}

int main(int argc, char *argv[])
{
    test();
    return 0;
}
