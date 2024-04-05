#include "tgpch.h"
#include "Scene.h"
#include "Entity.h"

#include "HotComponents/MaterialComponent.h"
#include "HotComponents/MaterialSystem.h"
#include "HotComponents/TransformComponent.h"
#include "Tupla/Core/Application.h"

namespace Tupla
{
	Scene::Scene()
	{
        // REGISTRATIONS HAVE TO HAPPEN AT THE SAME PLACE AND ORDER. ANY LOGIC HERE MAY RUIN OTHER SCENES!
        m_Coordinator.Init();
        m_Coordinator.RegisterComponent<MaterialComponent>();
        m_Coordinator.RegisterComponent<TransformComponent>();

        m_MatSystem = m_Coordinator.RegisterSystem<MaterialSystem>();
	}

    void Scene::CreateEntity()
    {
        m_Entities.emplace_back(this);
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
        // Run ECS update first
        auto& jobManager = Application::Get().GetJobManager();
		jobManager.ScheduleJob(MakeJob<MaterialSystem>(m_MatSystem, &m_Coordinator));
        jobManager.Await();

        // Run CS update after

    }
}
