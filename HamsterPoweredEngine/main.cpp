#include <iostream>
#include "Source/Engine/GameWindow.h"
#include "Source/Engine/Global.h"



int main()
{

    //global::Game = new GameWindow();
    //global::Game = GameWindow::instance();
    GameWindow::instance();

    return 0;
}