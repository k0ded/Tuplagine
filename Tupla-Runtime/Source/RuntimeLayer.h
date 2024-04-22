#pragma once
#include "Tupla/Core/Base.h"
#include "Tupla/Core/Layer.h"

namespace Tupla
{
    class Mesh;

	class RuntimeLayer : public Layer
    {
    public:
        RuntimeLayer();

        void OnUpdate() override;

	private:
        Ref<Mesh> myMesh;
    };
}
