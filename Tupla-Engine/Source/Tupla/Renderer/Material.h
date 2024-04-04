#pragma once
#include "Tupla/Core/Base.h"
#include <CommonUtilities/Math/Matrices/Matrix4x4.hpp>
#include <CommonUtilities/Math/Vector/Vector3.hpp>
#include "Shader.h"
#include "Texture.h"

namespace Tupla
{
    struct Constants
    {
        CU::Matrix4x4<float> Transform;
        CU::Matrix4x4<float> Projection;
        CU::Vector3f LightVector;
    };

    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void SetShaderStage(Ref<Shader> shader) = 0;
        virtual void AttachImage(Ref<Texture> texture) = 0;

        virtual void UpdateConstants(const Constants& consts) const = 0;
        virtual void AttachMaterial() const = 0;
    };
}
