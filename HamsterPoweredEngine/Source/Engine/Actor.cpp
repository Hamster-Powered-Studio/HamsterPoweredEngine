#include "Actor.h"
#include "Engine.h"
#include "Components/Components.h"



Actor::Actor(entt::entity handle, Scene* scene) : m_ActorHandle(handle), m_Scene(scene)
{
}



HPUUID Actor::GetUUID() const
{
    if (m_ActorHandle == entt::null)
        return 0;
    HPUUID id = GetComponent<IDComponent>().ID; 
    return id;
}


