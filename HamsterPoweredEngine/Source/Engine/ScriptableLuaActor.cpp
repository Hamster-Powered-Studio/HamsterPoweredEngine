#include "ScriptableLuaActor.h"



#include <fstream>
#include <SFML/OpenGL.hpp>
#include "LuaEngine.h"
#include "Components/Components.h"
#include "Actor.h"


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

void ScriptableLuaActor::OnUpdate(sf::Time deltaTime)
{
    ScriptableActor::OnUpdate(deltaTime);
    if (HasComponent<LuaScriptComponent>())
    {
        int index = 0;
        //lua["OnUpdate"](deltaTime.asSeconds());
        //lua.script("OnUpdate(" + std::to_string(deltaTime.asSeconds()) + ")");
        lua["OnUpdate"](deltaTime.asSeconds());
            
        for (std::string& script : m_scripts)
        {
            //chaiscript::ChaiScript chaiInstance;
            //chaiInstance.eval(script);
            //chaiInstance.eval("OnUpdate");
            
            //auto Update = chai.eval<std::function<void()>>("OnUpdate");
            //Update();
            //lua.do_file(script);
            //lua.script("OnUpdate()");
            index++;
        }
    }
}

void ScriptableLuaActor::BindLuaFunctions()
{
    
}
