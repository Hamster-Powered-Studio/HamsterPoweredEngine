#pragma once

#include "LuaEngine.h"
#include "ScriptableActor.h"



class ScriptableLuaActor : public ScriptableActor
{
public:
    ScriptableLuaActor()
    {
        
    }

    ~ScriptableLuaActor();
    virtual void ReloadScripts();
    
    void OnCreate() override;
    void OnDestroy() override;
    void OnUpdate(float deltaTime) override;

    void BindLuaFunctions();

    sol::state lua;
private:
    std::vector<std::string> m_scripts;
    std::function<void()> Update;
    
    
};
