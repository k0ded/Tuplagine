#pragma once
#include "Entity.h"

namespace Tupla
{
    class Scene
    {
    public:
        void CreateEntity();
        Entity* GetEntity(GUID id);
        void DeleteEntity(GUID id);

    private:
        std::vector<Entity> m_Entities;
    };
}
