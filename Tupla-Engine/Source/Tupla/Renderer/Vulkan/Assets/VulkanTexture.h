#pragma once
#include <vulkan/vulkan_core.h>

#include "Tupla/Renderer/Texture.h"
#include "Tupla/Renderer/Vulkan/VulkanDevice.h"

namespace Tupla
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(VulkanDevice& device): m_Device(device) {}
        ~VulkanTexture() override;
        void SetImageData(void* imageData, u32 texWidth, u32 texHeight, u32 texBytesPerPixel) override;
        void RecreateImage() override;

    protected:
        VulkanDevice& m_Device;
        VkImage m_Image {};
        VkDeviceMemory m_ImageMemory {};
    };
}
