#pragma once

#include "Actor.h"

class ScriptableActor
{
public:
    ScriptableActor() {}
    virtual ~ScriptableActor() {}
    
    template<typename T>
    T& GetComponent()
    {
        return m_Actor.GetComponent<T>();
    }

    template<typename T>
    bool HasComponent()
    {
        return m_Actor.HasComponent<T>();
    }

protected:
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnUpdate(float deltaTime) {}
    
protected:
    Actor m_Actor;
    friend class Scene;
};