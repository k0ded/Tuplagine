#pragma once
#include <vulkan/vulkan_core.h>

#include "VulkanWindow.h"

namespace Tupla
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    struct QueueFamilyIndices
    {
        uint32_t GraphicsFamily;
        uint32_t PresentFamily;
        bool GraphicsFamilyHasValue = false;
        bool PresentFamilyHasValue = false;
        [[nodiscard]] bool IsComplete() const { return GraphicsFamilyHasValue && PresentFamilyHasValue; }
    };
    
    class VulkanDevice
    {
    public:
#ifdef DIST
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        VulkanDevice(VulkanWindow& window);
        ~VulkanDevice();

        // Not copyable or movable
        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;
        VulkanDevice(VulkanDevice&&) = delete;
        VulkanDevice& operator=(VulkanDevice&&) = delete;

        [[nodiscard]] VkCommandPool GetCommandPool() const { return m_CommandPool; }
        [[nodiscard]] VkDevice Device() const { return m_Device; }
        [[nodiscard]] VkSurfaceKHR Surface() const { return m_Surface; }
        [[nodiscard]] VkQueue GraphicsQueue() const { return m_GraphicsQueue; }
        [[nodiscard]] VkQueue PresentQueue() const { return m_PresentQueue; }

        SwapChainSupportDetails GetSwapChainSupport() const { return QuerySwapChainSupport(m_PhysicalDevice); }
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
        [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

        // Buffer Helper Functions
        void CreateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);
        [[nodiscard]] VkCommandBuffer BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) const;

        void CreateImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory);

        VkPhysicalDevice GetPhysicalDevice() const;

        VkPhysicalDeviceProperties Properties;

        VkInstance Instance;
    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        // helper functions
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> GetRequiredExtensions() const;
        bool CheckValidationLayerSupport();
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void HasGflwRequiredInstanceExtensions();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;

        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VulkanWindow& m_Window;
        VkCommandPool m_CommandPool;

        VkDevice m_Device;
        VkSurfaceKHR m_Surface;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        const std::vector<const char*> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };
}
