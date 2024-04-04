﻿#pragma once
#include "Tupla/Core/Base.h"
#include "Tupla/Core/Layer.h"
#include "Tupla/Renderer/Material.h"
#include "Tupla/Renderer/Mesh.h"

namespace Tupla
{
    class RuntimeLayer : public Layer
    {
    public:
        RuntimeLayer();

        void OnUpdate() override;

    private:
        Ref<Mesh> m_Mesh;
        Ref<Material> m_Material;
        Ref<Texture> m_Texture;

        CU::Matrix4x4<float> m_Transform;
    };
}
