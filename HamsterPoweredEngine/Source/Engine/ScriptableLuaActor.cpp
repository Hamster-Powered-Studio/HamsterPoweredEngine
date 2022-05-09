#include "ScriptableLuaActor.h"



#include <fstream>
#include <SFML/OpenGL.hpp>
#include "LuaEngine.h"
#include "Components/Components.h"
#include "Actor.h"
#include "Global.h"
#include "Components/LuaBindings.h"
#include "iostream"

ScriptableLuaActor::~ScriptableLuaActor()
{
    OnDestroy();
}

void ScriptableLuaActor::ReloadScripts()
{
    if (HasComponent<LuaScriptComponent>())
    {
        if (!GetComponent<LuaScriptComponent>().Scripts.size() > 0) return;
        auto& script = GetComponent<LuaScriptComponent>().Scripts[0];
        
        std::ifstream ifs(script);
        if (ifs)
        {
            std::string content( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
            if (content.compare( 0, 3, "\xEF\xBB\xBF" ) == 0)  // Is the file marked as UTF-8?
                {
                
                content.erase( 0, 3 );                  // Now get rid of the BOM.
                std::ofstream ofs(script);
                ofs << content;                                    // Fix file
                }

            LuaBindings::RegisterBindings(lua, m_Actor);
            //lua["Self"] = Actor(m_Actor);
            //lua["trans"] = &m_Actor.GetComponent<TransformComponent>();
            lua.script_file(script);

            lua.script("OnCreate()");
            
            
        }
    }
}


void ScriptableLuaActor::OnCreate()
{
    ScriptableActor::OnCreate();
    
    BindLuaFunctions();
    
    ReloadScripts();
    if (HasComponent<LuaScriptComponent>())
    {
        int index = 0;
        for (std::string& script : m_scripts)
        {

            //chai.eval(script);
            //lua.do_file(script);
            //lua.script("OnCreate()");
            index++;
        }
    }
}

void ScriptableLuaActor::OnDestroy()
{
    ScriptableActor::OnDestroy();
    for (std::string& script : m_scripts)
    {

        //chai.eval(script);
        //lua.do_file(script);
        //lua.script("OnDestroy()");
    }
}

void ScriptableLuaActor::OnUpdate(float deltaTime)
{
    ScriptableActor::OnUpdate(deltaTime);
    if (HasComponent<LuaScriptComponent>())
    {
        
        int index = 0;
        lua["OnUpdate"](deltaTime);
    }
}

void ScriptableLuaActor::BindLuaFunctions()
{
    
}
