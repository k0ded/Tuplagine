#pragma once
#include "Tupla/Core/Base.h"
#include "Tupla/Core/Layer.h"
#include "Tupla/Renderer/Camera.h"
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

        CU::Matrix4x4<float> m_Transform;

        Camera m_Camera;

        CU::Vector3f m_CameraPosition;
        CU::Vector3f m_CameraRotation;
    };
}
