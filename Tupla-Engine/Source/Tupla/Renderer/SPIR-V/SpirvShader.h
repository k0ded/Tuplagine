#pragma once
#include "Tupla/Renderer/Shader.h"

namespace Tupla
{
    class SpirvShader : public Shader
    {
    public:
        Scope<std::string> GetCode() override;
    };
}
