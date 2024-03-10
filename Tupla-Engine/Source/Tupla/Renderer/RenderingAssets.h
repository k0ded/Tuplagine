#pragma once
#include "Texture.h"
#include "Buffer.h"
#include "Mesh.h"

namespace Tupla
{
    class RenderingAssets
    {
    public:
        virtual ~RenderingAssets() = default;
        virtual Scope<Buffer> CreateBuffer(u64 instanceSize, u32 instanceCount, i32 bUsage, i32 mUsage) = 0;
        virtual Scope<Texture> CreateTexture() = 0;
        virtual Scope<Mesh> CreateMesh() = 0;
    };
}
