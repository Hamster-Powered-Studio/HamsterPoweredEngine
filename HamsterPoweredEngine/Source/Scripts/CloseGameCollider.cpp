#include "CloseGameCollider.h"

#include "Components/Components.h"

void CloseGameCollider::OnUpdate(sf::Time deltaTime)
{
    ScriptableActor::OnUpdate(deltaTime);
    auto& Collider = GetComponent<BoxColliderComponent>();

    if (Collider.Other.m_ActorHandle == entt::null) return;
    if (!Collider.Other.HasComponent<TagComponent>()) return;
    if (Collider.Other.GetComponent<TagComponent>().Tag == "Player")
    {
        system("pause");
    }
    
    
}
