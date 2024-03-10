#pragma once
#include "VulkanTexture.h"
#include "Tupla/Renderer/Vulkan/VulkanSwapChain.h"

namespace Tupla
{
    class VulkanRenderTexture final : public VulkanTexture
    {
    public:
        explicit VulkanRenderTexture(VulkanDevice& device, const VulkanSwapChain& swapChain);
        ~VulkanRenderTexture() override;

        VkImageView GetImageView() const { return m_ImageView; }
        VkFramebuffer GetFramebuffer() const { return m_Framebuffer; }

    private:
        VkImageView m_ImageView;
        VkFramebuffer m_Framebuffer;
    };
}
