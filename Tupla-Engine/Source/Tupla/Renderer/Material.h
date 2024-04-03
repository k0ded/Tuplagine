#pragma once
#include "Tupla/Utils/Hashing.h"
#include "Tupla/Core/Base.h"
#include "Shader.h"

namespace Tupla
{
    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void SetShader(Ref<Shader> shader) = 0;
    };
}
