#pragma once
#include "Tupla/Core/Layer.h"

namespace Tupla
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer() = default;
        void OnGUI() override;

        void OnDetach() override;
    };
}
