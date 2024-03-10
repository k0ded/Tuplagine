#pragma once
#include "Tupla/Renderer/Buffer.h"
#include "Tupla/Renderer/Mesh.h"
#include "Tupla/Renderer/RenderingAssets.h"
#include "Tupla/Renderer/Vulkan/VulkanDevice.h"

namespace Tupla
{
    class VulkanRenderingAssets : public RenderingAssets
    {
    public:
        VulkanRenderingAssets(VulkanDevice& device): m_Device(device) {}
        
        Scope<Buffer> CreateBuffer(u64 instanceSize, u32 instanceCount, i32 bUsage, i32 mUsage) override;
        Scope<Texture> CreateTexture() override;
        Scope<Mesh> CreateMesh() override;

    private:
        VulkanDevice& m_Device;
    };
}
