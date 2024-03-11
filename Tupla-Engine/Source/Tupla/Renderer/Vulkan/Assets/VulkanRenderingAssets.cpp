#include "tgpch.h"
#include "VulkanRenderingAssets.h"

#include "VulkanBuffer.h"
#include "VulkanMesh.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

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

    Scope<Shader> VulkanRenderingAssets::CreateShader(const std::string& virtualPath)
    {
        return CreateScope<VulkanShader>(m_Device, virtualPath);
    }

    Scope<Mesh> VulkanRenderingAssets::CreateMesh()
    {
        return CreateScope<VulkanMesh>(m_Device);
    }
}
