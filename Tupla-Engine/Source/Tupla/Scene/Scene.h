#pragma once
#include "Entity.h"
#include "Hot/HotCoordinator.h"
#include "HotComponents/MaterialSystem.h"
#include "Tupla/Jobs/Job.h"

namespace Tupla
{
	class Scene
    {
    public:
        Scene();

        void CreateEntity();
        Entity* GetEntity(GUID id);
        void DeleteEntity(GUID id);

        // Behaviour Functions
        void Update();


        HotCoordinator& GetHOT() { return m_Coordinator; }
    private:
        template <typename T>
        Job MakeJob(std::shared_ptr<T>& system, HotCoordinator* coordinator)
        {
            auto r = m_Coordinator.GetSystemReadAccesses<T>();
            auto w = m_Coordinator.GetSystemWriteAccesses<T>();
            return Job([this, system, coordinator] { system->Update(coordinator); });
        }

        std::vector<Entity> m_Entities;

        std::shared_ptr<MaterialSystem> m_MatSystem;
        HotCoordinator m_Coordinator;
    };
}
