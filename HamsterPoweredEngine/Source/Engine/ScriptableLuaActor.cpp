#include "ScriptableLuaActor.h"

#include <fstream>
#include <SFML/OpenGL.hpp>

#include "Components/Components.h"


void ScriptableLuaActor::ReloadScripts()
{
    if (HasComponent<LuaScriptComponent>())
    {
        auto& scriptList = GetComponent<LuaScriptComponent>().Scripts;
        m_scripts.clear();
        int index = 0;
        for (std::string& script : scriptList)
        {
            std::ifstream ifs(script);
            std::string content( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
            if (content.compare( 0, 3, "\xEF\xBB\xBF" ) == 0)  // Is the file marked as UTF-8?
                {
                
                content.erase( 0, 3 );                  // Now get rid of the BOM.
                std::ofstream ofs(script);
                ofs << content;                                    // Fix file
                }
            m_scripts.emplace_back(content);
            index++;
        }
    }
}

void ScriptableLuaActor::OnCreate()
{
    ScriptableActor::OnCreate();
    
    ReloadScripts();
    if (HasComponent<LuaScriptComponent>())
    {
        int index = 0;
        for (std::string& script : m_scripts)
        {
            lua.script(m_scripts[index]);
            lua.script("OnCreate()");
            lua.stack_clear();
            index++;
        }
    }
}

void ScriptableLuaActor::OnDestroy()
{
    ScriptableActor::OnDestroy();
    for (std::string& script : m_scripts)
    {
        lua.script_file(script);
        lua.script("OnDestroy()");
        //lua.stack_clear();
    }
}

void ScriptableLuaActor::OnUpdate(sf::Time deltaTime)
{
    ScriptableActor::OnUpdate(deltaTime);
    if (HasComponent<LuaScriptComponent>())
    {
        int index = 0;
        for (std::string& script : m_scripts)
        {
            lua.script(m_scripts[index]);
            lua.script("OnUpdate()");
            lua.stack_clear();
            index++;
        }
    }
    
}
