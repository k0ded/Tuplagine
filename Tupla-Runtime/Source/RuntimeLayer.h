#pragma once
#include "Tupla/Core/Base.h"
#include "Tupla/Core/Layer.h"
#include "Tupla/Renderer/Material.h"

namespace Tupla
{
	class Model;

	class RuntimeLayer : public Layer
    {
    public:
        RuntimeLayer();

        void OnUpdate() override;

    private:
        Ref<Model> m_Mesh;
        Ref<Material> m_Material;
        Ref<Texture> m_Texture;
        Ref<Buffer> m_ConstantBuffer;
        Ref<Buffer> m_CameraBuffer;

        CU::Matrix4x4<float> m_Transform;
        CU::Matrix4x4<float> m_CameraTransform;
    };
}
