#pragma once
#include "Texture.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Shader.h"

namespace Tupla
{
    class RenderingAssets
    {
    public:
        virtual ~RenderingAssets() = default;
        virtual Scope<Shader> CreateShader(const std::string& virtualPath, ShaderStage stage) = 0;
        virtual Scope<Texture> CreateTexture() = 0;
        virtual Scope<Mesh> CreateMesh() = 0;
    };
}
