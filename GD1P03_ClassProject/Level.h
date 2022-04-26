#pragma once
#include <string>
#include "vector"
#include "Actor.h"

/*
class Level
{
public:
    std::string LevelName;
    Level(std::string path);
    ~Level();


    template <class T>
    std::enable_if_t<std::is_base_of_v<Actor, T>, Actor*> SpawnActor(sf::Vector2f location = sf::Vector2f(0, 0), int index = 0);

    bool SaveLevel(std::string path);

    bool LoadLevel(std::string path);

    void Update();

    void Begin();

    std::vector<Actor*> Actors;
};
*/