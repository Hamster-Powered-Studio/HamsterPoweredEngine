#pragma once
#include "Scene.h"
#include "entt.hpp"


class Actor {
public:
    Actor() = default;
    Actor(entt::entity handle, Scene* scene);
    Actor(const Actor& other) = default;

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        assert(!HasComponent<T>(), "Entity already has component!");

        T& component = m_Scene->Registry.emplace<T>(m_ActorHandle, std::forward<Args>(args)...);
        m_Scene->OnComponentAdded<T>(*this, component);
        return component;
    }

    template<typename T>
    T& GetComponent()
    {
        assert(HasComponent<T>(), "Entity does not have component!");
        return m_Scene->Registry.get<T>(m_ActorHandle);
    }

    template<typename T>
    bool HasComponent()
    {

        return m_Scene->Registry.any_of<T>(m_ActorHandle);
       
    }

    template<typename T>
    void RemoveComponent()
    {
        m_Scene->Registry.remove<T>(m_ActorHandle);
    }

    operator bool() const { return m_ActorHandle != entt::null; }
    operator uint32_t() const { return (uint32_t)m_ActorHandle; }
    bool operator==(const Actor& other) const { return m_ActorHandle == other.m_ActorHandle && m_Scene == other.m_Scene; }
    bool operator!=(const Actor& other) const { return !(*this == other); }
    operator entt::entity() const{ return m_ActorHandle; }

    std::list<Actor> children;
    entt::entity m_ActorHandle{ entt::null };
private:
    Actor* parent;
    
    Scene* m_Scene = nullptr;
};

/*      OLD

class Actor : public sf::Sprite
{
public:
    sf::Texture Texture; 
    std::string texturePath;
    std::string displayName = "Actor";
    Actor();
    Actor(sf::Texture& texture);
    Actor(std::string texturePath);
    ~Actor();
    
    bool LoadTexture(std::string path);
    bool LoadTexture(sf::Texture& texture);

    virtual void Update();

    std::string GetClassName();

    bool visible = true;

    void Attach(Actor* NewParent, bool SnapToParent = true);
    void Detach();

    sf::Vector2f GetSize();
    void SetRelativePosition(Actor& relativeTo, sf::Vector2f pos);

    sf::Vector2f GetRelativePosition(Actor& relativeTo);

    virtual void DisplayExposedVars();

    void AddWorldOffset(sf::Vector2f offset);
    


    int InstanceNum;

    static inline int ActorNum;
    static inline int ActorMax;

    Actor* parent;
    std::vector<Actor*> children;
    sf::Vector2f offsetFromParent;
    
    double hRatio = getScale().y / getScale().x;
    double wRatio = getScale().x / getScale().y;

    bool lockScale;
    bool snapToParent;

private:
    float width, height;
    
};

*/