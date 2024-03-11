#pragma once
#include "VulkanTexture.h"
#include "Tupla/Renderer/Vulkan/VulkanSwapChain.h"

namespace Tupla
{
    class VulkanRenderTexture
    {
    public:
        explicit VulkanRenderTexture(VulkanDevice& device, const VulkanSwapChain& swapChain);
        ~VulkanRenderTexture();

        void RecreateTexture(const VulkanSwapChain& swapChain);
        
        VkImageView GetImageView(u32 index) const { return m_ColorImageView[index]; }
        VkFramebuffer GetFramebuffer(u32 index) const { return m_Framebuffer[index]; }
        VkRenderPass GetRenderPass() const { return m_RenderPass; }

    private:

        void CreateImages(const VulkanSwapChain& swapChain);
        void CreateFrameBuffer(const VulkanSwapChain& swapChain);
        void CreateRenderPass(const VulkanSwapChain& swapChain);
        
        VulkanDevice& m_Device;
        
        std::vector<VulkanTexture> m_Color;
        std::vector<VkImageView> m_ColorImageView;
        
        std::vector<VkFramebuffer> m_Framebuffer;
        VkRenderPass m_RenderPass;
    };
}
