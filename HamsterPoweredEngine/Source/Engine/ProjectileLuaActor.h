#pragma once
#include "Engine/ScriptableLuaActor.h"

class ProjectileLuaActor : public ScriptableLuaActor
{
public:
    void ReloadScripts() override
    {
        ScriptableLuaActor::ReloadScripts();

    }
    
};
