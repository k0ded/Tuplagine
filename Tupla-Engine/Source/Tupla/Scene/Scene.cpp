#include "tgpch.h"
#include "Scene.h"
#include "Entity.h"

#include "Tupla/Core/Application.h"

namespace Tupla
{
	Scene::Scene()
	{
	}

    Entity& Scene::CreateEntity()
    {
        m_Entities.emplace_back(this);
        return m_Entities.back();
    }

    Entity* Scene::GetEntity(const GUID id)
    {
        for (auto& entity : m_Entities)
        {
            if(entity.GetId() == id)
            {
                return &entity;
            }
        }

        return nullptr;
    }

    void Scene::DeleteEntity(const GUID id)
    {
        for (int i = 0; i < m_Entities.size(); ++i)
        {
            if(m_Entities[i].GetId() == id)
            {
                m_Entities.erase(m_Entities.begin() + i);
                break;
            }
        }
    }

    void Scene::Update()
    {
        // Run CS update

    }
}
