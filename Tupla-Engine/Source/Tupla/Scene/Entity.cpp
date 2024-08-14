#include "tgpch.h"
#include "Entity.h"
#include "Component/IComponent.h"
#include "Scene.h"

Tupla::Entity::Entity(Scene* aScene)
{
	CoCreateGuid(&m_Id);
	m_Scene = aScene;
}

void Tupla::Entity::RemoveComponent(const GUID aId)
{
    for (auto i = m_Components.begin(); i != m_Components.end(); ++i)
    {
        if (i->get()->GetId() == aId)
        {
            m_Components.erase(i);
            break;
        }
    }
}
