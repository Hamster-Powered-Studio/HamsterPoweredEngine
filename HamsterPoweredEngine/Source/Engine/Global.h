#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include "GameWindow.h"


#ifndef NDEBUG
// Production builds should set NDEBUG=1
#define NDEBUG false
#else
#define NDEBUG true
#endif

#ifndef EDITOR
#define EDITOR !NDEBUG
#endif

namespace global
{
    extern sf::Event* event;
    extern GameWindow* Game;
    extern sf::Clock deltaClock;
    extern float deltaTime;
    extern bool isInPlay;


}
