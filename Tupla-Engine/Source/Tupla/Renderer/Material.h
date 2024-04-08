#pragma once
#include "Tupla/Core/Base.h"
#include <CommonUtilities/Math/Matrices/Matrix4x4.hpp>
#include <CommonUtilities/Math/Vector/Vector3.hpp>

#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Tupla
{
    struct Constants
    {
        CU::Matrix4x4<float> Transform;
        CU::Vector3f LightVector;
    };

    struct CameraData
    {
        CU::Matrix4x4<float> worldToClip;
        CU::Matrix4x4<float> worldToCamera;
    };

    struct BufferBinding
    {
        ShaderStage Stage;
        Ref<Buffer> ConstBuffer;
    };

    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void SetShader(Ref<Shader> shader) = 0;
        virtual void AttachImage(Ref<Texture> texture) = 0;
        virtual void AttachBuffer(Ref<Buffer> buffer, ShaderStageSlot stage) = 0;
        virtual void BindMaterial() const = 0;
    };
}
