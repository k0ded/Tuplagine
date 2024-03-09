#pragma once
#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"

namespace Tupla
{
    enum class SwapChainPriority
    {
        LowLatency,
        NoTearing,
        LowPowerConsumption
    };
    
    class VulkanSwapChain
    {
    public:
        static constexpr int MaxFramesInFlight = 2;

        VulkanSwapChain(VulkanDevice& deviceRef, VkExtent2D windowExtent, const VulkanSwapChain* oldSwapChain = nullptr, SwapChainPriority priority = SwapChainPriority::LowLatency);
        ~VulkanSwapChain();

        VulkanSwapChain(const VulkanSwapChain&) = delete;
        VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

        [[nodiscard]] VkFramebuffer GetFrameBuffer(const int index) const { return m_SwapChainFrameBuffers[index]; }
        [[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass; }
        [[nodiscard]] VkImageView GetImageView(const int index) const { return m_SwapChainImageViews[index]; }
        [[nodiscard]] size_t ImageCount() const { return m_SwapChainImages.size(); }
        [[nodiscard]] VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
        [[nodiscard]] VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }
        [[nodiscard]] uint32_t Width() const { return m_SwapChainExtent.width; }
        [[nodiscard]] uint32_t Height() const { return m_SwapChainExtent.height; }

        [[nodiscard]] float ExtentAspectRatio() const
        {
            return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
        }

        [[nodiscard]] VkFormat FindDepthFormat() const;

        VkResult AcquireNextImage(uint32_t* imageIndex) const;
        VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, const uint32_t* imageIndex);

        [[nodiscard]] bool CompareSwapFormats(const VulkanSwapChain& swapChain) const
        {
            return  swapChain.m_SwapChainDepthFormat == m_SwapChainDepthFormat &&
                    swapChain.m_SwapChainImageFormat == m_SwapChainImageFormat;
        }

    private:
        void CreateSwapChain(SwapChainPriority priority, const VulkanSwapChain* oldSwapChain);
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFrameBuffers();
        void CreateSyncObjects();

        // Helper functions
        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, SwapChainPriority priority);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

        VkFormat m_SwapChainImageFormat;
        VkFormat m_SwapChainDepthFormat;
        VkExtent2D m_SwapChainExtent;

        std::vector<VkFramebuffer> m_SwapChainFrameBuffers;
        VkRenderPass m_RenderPass;

        std::vector<VkImage> m_DepthImages;
        std::vector<VkDeviceMemory> m_DepthImageMemories;
        std::vector<VkImageView> m_DepthImageViews;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        VulkanDevice& m_Device;
        VkExtent2D m_WindowExtent;

        VkSwapchainKHR m_SwapChain;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;
        size_t m_CurrentFrame = 0;
    };
}
