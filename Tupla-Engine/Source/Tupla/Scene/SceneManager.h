#pragma once
#include "Scene.h"

namespace Tupla
{
    class SceneManager
    {
    public:
        static Scene& GetActiveScene() { return m_ActiveScene; }

    private:
        static inline Scene m_ActiveScene {};
    };
}
