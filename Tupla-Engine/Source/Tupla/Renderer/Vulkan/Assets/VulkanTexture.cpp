#include "tgpch.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan_core.h>

#include "Tupla/Core/Application.h"
#include "VulkanBuffer.h"

namespace Tupla
{
    VulkanTexture::~VulkanTexture()
    {
        vkDestroyImage(m_Device.Device(), m_Image, nullptr);
        vkFreeMemory(m_Device.Device(), m_ImageMemory, nullptr);
    }

    void VulkanTexture::SetImageData(void* imageData, const u32 texWidth, const u32 texHeight, const u32 texBytesPerPixel)
    {
        const u64 imageSize = texWidth * texHeight * texBytesPerPixel;
        const Scope<VulkanBuffer> staging = CreateScope<VulkanBuffer>(
                                m_Device,
                                imageSize, 1,
                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        staging->Map();
        memcpy(staging->GetMappedMemory(), imageData, imageSize);
        staging->Unmap();

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(texWidth);
        imageInfo.extent.height = static_cast<uint32_t>(texHeight);
        imageInfo.extent.depth = 1; // NO MIPMAPPING RIGHT NOW!
        imageInfo.mipLevels = 1; // NO MIPMAPPING RIGHT NOW!
        imageInfo.arrayLayers = 1;
        
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        m_Device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);
    }

    void VulkanTexture::RecreateImage()
    {
        // TODO: Implement this so that you can quickly reload images
    }
}
