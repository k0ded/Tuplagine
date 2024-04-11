#pragma once
#include "Assets.h"
#include "Tupla/Core/Layer.h"

namespace Tupla
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer() = default;
        void OnGUI() override;

        void OnDetach() override;

    private:
        Assets m_Assets;
    };
}
