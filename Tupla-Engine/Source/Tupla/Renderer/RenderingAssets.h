#pragma once
#include "Buffer.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

namespace Tupla
{
    class RenderingAssets
    {
    public:
        virtual ~RenderingAssets() = default;
        virtual Scope<Material> CreateMaterial() = 0;
        virtual Scope<Shader> CreateShader(const std::wstring& virtualPath, ShaderStage stage) = 0;
        virtual Scope<Texture> CreateTexture() = 0;
        virtual Scope<Mesh> CreateMesh() = 0;
        virtual Scope<Buffer> CreateBuffer(u32 byteSize) = 0;
    };
}
