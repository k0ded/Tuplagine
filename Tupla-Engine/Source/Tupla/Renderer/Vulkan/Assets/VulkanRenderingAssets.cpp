#include "tgpch.h"
#include "VulkanRenderingAssets.h"

#include "VulkanBuffer.h"
#include "VulkanMesh.h"
#include "VulkanTexture.h"

namespace Tupla
{
    Scope<Buffer> VulkanRenderingAssets::CreateBuffer(u64 instanceSize, u32 instanceCount, i32 bUsage, i32 mUsage)
    {
        return CreateScope<VulkanBuffer>(
            m_Device,
            instanceSize,
            instanceCount,
            bUsage,
            mUsage
            );
    }

    Scope<Texture> VulkanRenderingAssets::CreateTexture()
    {
        return CreateScope<VulkanTexture>(m_Device);
    }

    Scope<Mesh> VulkanRenderingAssets::CreateMesh()
    {
        return CreateScope<VulkanMesh>(m_Device);
    }
}
