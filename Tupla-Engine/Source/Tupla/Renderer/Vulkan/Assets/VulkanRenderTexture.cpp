#include "tgpch.h"
#include "VulkanRenderTexture.h"

#include "Tupla/Renderer/Vulkan/VulkanSwapChain.h"

namespace Tupla
{
    VulkanRenderTexture::VulkanRenderTexture(VulkanDevice& device, const VulkanSwapChain& swapChain):
        VulkanTexture(device), m_ImageView(nullptr), m_Framebuffer(nullptr)
    {
        // Create the linear tiled destination image to copy to and to read the memory from
        VkImageCreateInfo imageCreateCI{};
        imageCreateCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateCI.imageType = VK_IMAGE_TYPE_2D;
        // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
        imageCreateCI.format = VK_FORMAT_B8G8R8A8_SRGB;
        imageCreateCI.extent.width = swapChain.Width();
        imageCreateCI.extent.height = swapChain.Height();
        imageCreateCI.extent.depth = 1;
        imageCreateCI.arrayLayers = 1;
        imageCreateCI.mipLevels = 1;
        imageCreateCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateCI.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateCI.tiling = VK_IMAGE_TILING_LINEAR;
        imageCreateCI.usage =  VK_IMAGE_USAGE_SAMPLED_BIT;
        // Create the image
        // VkImage dstImage;
        vkCreateImage(m_Device.Device(), &imageCreateCI, nullptr, &m_Image);
        // Create memory to back up the image
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo memAllocInfo{};
        memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        // VkDeviceMemory dstImageMemory;
        vkGetImageMemoryRequirements(m_Device.Device(), m_Image, &memRequirements);
        memAllocInfo.allocationSize = memRequirements.size;
        // Memory must be host visible to copy from
        memAllocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits,
                                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkAllocateMemory(m_Device.Device(), &memAllocInfo, nullptr, &m_ImageMemory);
        vkBindImageMemory(m_Device.Device(), m_Image, m_ImageMemory, 0);

        const VkCommandBuffer copyCmd = m_Device.BeginSingleTimeCommands();
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageMemoryBarrier.image = m_Image;
        imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        vkCmdPipelineBarrier(
            copyCmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier
        );

        m_Device.EndSingleTimeCommands(copyCmd);

        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_Image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_Device.Device(), &createInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }

    VulkanRenderTexture::~VulkanRenderTexture()
    {
        vkDestroyImageView(m_Device.Device(), m_ImageView, nullptr);
        vkDestroyFramebuffer(m_Device.Device(), m_Framebuffer, nullptr);
    }
}
