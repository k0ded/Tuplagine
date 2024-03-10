#include "tgpch.h"
#include "Scene.h"

namespace Tupla
{
    void Scene::CreateEntity()
    {
        m_Entities.emplace_back();
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
}
