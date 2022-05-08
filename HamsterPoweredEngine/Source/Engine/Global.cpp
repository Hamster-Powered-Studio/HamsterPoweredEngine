#include "Global.h"

namespace global
{
    sf::Event* event = nullptr;
    GameWindow* Game = nullptr;
    sf::Clock deltaClock;
    float deltaTime;
    bool isInPlay = NDEBUG;

}
