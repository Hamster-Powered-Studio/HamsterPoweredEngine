#include "Global.h"

namespace global
{
    sf::Event* event = nullptr;
    GameWindow* Game = nullptr;
    sf::Clock deltaClock;
    sf::Time deltaTime;
    bool isInPlay = NDEBUG;
    //Level* World;

}
