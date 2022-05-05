#pragma once

#include "ScriptableActor.h"
#include "sol/sol.hpp"

class ScriptableLuaActor : public ScriptableActor
{
public:
    ScriptableLuaActor()
    {
        lua.open_libraries(sol::lib::base);
    }

    void ReloadScripts();
    
    void OnCreate() override;
    void OnDestroy() override;
    void OnUpdate(sf::Time deltaTime) override;

    
private:
    sol::state lua;
    std::vector<std::string> m_scripts;
};
