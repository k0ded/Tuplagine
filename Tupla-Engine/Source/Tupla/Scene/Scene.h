#pragma once
#include "Entity.h"

namespace Tupla
{
	class Scene
    {
    public:
        Scene();

        Entity& CreateEntity();
        Entity* GetEntity(GUID id);
        void DeleteEntity(GUID id);

        // Behaviour Functions
        void Update();

        std::vector<Entity>& GetEntities() { return m_Entities; }

    private:
        std::vector<Entity> m_Entities;
    };
}
