#pragma once

#include "LuaEngine.h"
#include "ScriptableActor.h"



class ScriptableLuaActor : public ScriptableActor
{
public:
    ScriptableLuaActor()
    {
        
    }

    void ReloadScripts();
    
    void OnCreate() override;
    void OnDestroy() override;
    void OnUpdate(sf::Time deltaTime) override;

    void BindLuaFunctions();

    
private:
    std::vector<std::string> m_scripts;
    std::function<void()> Update;
    sol::state& lua = LuaEngine::lua;
    
};
