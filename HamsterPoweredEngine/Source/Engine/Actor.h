#pragma once
#include "Scene.h"
#include "entt.hpp"
//#include "Components/Components.h"


struct Actor {
public:
    Actor() {}
    Actor(entt::entity handle, Scene* scene);
    Actor(const Actor& other) : m_ActorHandle(other.m_ActorHandle), m_Scene(other.m_Scene) {};
    //: m_ActorHandle(other.m_ActorHandle), m_Scene(other.m_Scene){}

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        T& component = m_Scene->Registry.emplace<T>(m_ActorHandle, std::forward<Args>(args)...);
        m_Scene->OnComponentAdded<T>(*this, component);
        return component;
    }

    template<typename T>
    T& GetComponent() const
    {
        return m_Scene->Registry.get<T>(m_ActorHandle);
    }

    template<typename T>
    bool HasComponent() const
    {
        return m_Scene->Registry.any_of<T>(m_ActorHandle);
    }

    template<typename T>
    static T& GetComponentStatic(Actor handle)
    {
        return handle.m_Scene->Registry.get<T>(handle);
    }

    template<typename T>
    static bool HasComponentStatic(Actor handle)
    {
        return handle.m_Scene->Registry.any_of<T>(handle);
    }
    
    
    template<typename T>
    void RemoveComponent()
    {
        m_Scene->Registry.remove<T>(m_ActorHandle);
    }
    
    Scene* GetScene()
    {
        return m_Scene;
    }

    
    
    HPUUID GetUUID() const;
    
    operator bool() const { return m_ActorHandle != entt::null; }
    operator uint32_t() const { return (uint32_t)m_ActorHandle; }
    bool operator==(const Actor& other) const { return m_ActorHandle == other.m_ActorHandle && m_Scene == other.m_Scene; }
    bool operator!=(const Actor& other) const { return !(*this == other); }
    operator entt::entity() const{ return m_ActorHandle; }


    entt::entity m_ActorHandle{ entt::null };

private:
    Scene* m_Scene = nullptr;
};

