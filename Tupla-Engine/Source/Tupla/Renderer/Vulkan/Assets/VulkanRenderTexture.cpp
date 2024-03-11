#include "tgpch.h"
#include "VulkanRenderTexture.h"

#include "Tupla/Renderer/Vulkan/VulkanSwapChain.h"

namespace Tupla
{
    VulkanRenderTexture::VulkanRenderTexture(VulkanDevice& device, const VulkanSwapChain& swapChain):
        m_Device(device)
    {
        RecreateTexture(swapChain);
    }

    VulkanRenderTexture::~VulkanRenderTexture()
    {
        for (int i = 0; i < m_ColorImageView.size(); ++i)
        {
            vkDestroyImageView(m_Device.Device(), m_ColorImageView[i], nullptr);
            vkDestroyFramebuffer(m_Device.Device(), m_Framebuffer[i], nullptr);
        }

        vkDestroyRenderPass(m_Device.Device(), m_RenderPass, nullptr);
    }

    void VulkanRenderTexture::RecreateTexture(const VulkanSwapChain& swapChain)
    {
        CreateImages(swapChain);
        CreateRenderPass(swapChain);
        CreateFrameBuffer(swapChain);
    }

    void VulkanRenderTexture::CreateImages(const VulkanSwapChain& swapChain)
    {
        m_Color.clear();
        m_ColorImageView.resize(swapChain.ImageCount());
    	m_Color.reserve(swapChain.ImageCount());
        
        for (int i = 0; i < swapChain.ImageCount(); ++i)
        {
            m_Color.emplace_back(m_Device);

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
            imageCreateCI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            // Create the image
            // VkImage dstImage;
            vkCreateImage(m_Device.Device(), &imageCreateCI, nullptr, &m_Color[i].GetImage());
            // Create memory to back up the image
            VkMemoryRequirements memRequirements;
            VkMemoryAllocateInfo memAllocInfo{};
            memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            // VkDeviceMemory dstImageMemory;
            vkGetImageMemoryRequirements(m_Device.Device(), m_Color[i].GetImage(), &memRequirements);
            memAllocInfo.allocationSize = memRequirements.size;
            // Memory must be host visible to copy from
            memAllocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits,
                                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkAllocateMemory(m_Device.Device(), &memAllocInfo, nullptr, &m_Color[i].GetMemory());
            vkBindImageMemory(m_Device.Device(), m_Color[i].GetImage(), m_Color[i].GetMemory(), 0);

            const VkCommandBuffer copyCmd = m_Device.BeginSingleTimeCommands();
            VkImageMemoryBarrier imageMemoryBarrier{};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageMemoryBarrier.image = m_Color[i].GetImage();
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
            createInfo.image = m_Color[i].GetImage();
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

            if (vkCreateImageView(m_Device.Device(), &createInfo, nullptr, &m_ColorImageView[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void VulkanRenderTexture::CreateFrameBuffer(const VulkanSwapChain& swapChain)
    {
        m_Framebuffer.resize(swapChain.ImageCount());
        for (int i = 0; i < swapChain.ImageCount(); ++i)
        {
            std::array attachments = { m_ColorImageView[i], swapChain.GetDepthView(i) };
            const auto [width, height] = swapChain.GetSwapChainExtent();
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            if(vkCreateFramebuffer(m_Device.Device(), &framebufferInfo, nullptr, &m_Framebuffer[i]) != VK_SUCCESS)
            {
	            throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }

    void VulkanRenderTexture::CreateRenderPass(const VulkanSwapChain& swapChain)
    {
        std::array<VkAttachmentDescription, 2> attachments = {};
			// Color attachment
			attachments[0].format = swapChain.GetSwapChainImageFormat();
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// Depth attachment
			attachments[1].format = swapChain.FindDepthFormat();
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference colorReference = {};
			colorReference.attachment = 0;
			colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthReference = {};
			depthReference.attachment = 1;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorReference;
			subpassDescription.pDepthStencilAttachment = &depthReference;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;
			subpassDescription.pResolveAttachments = nullptr;

			// Subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpassDescription;
			renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassInfo.pDependencies = dependencies.data();

			if (vkCreateRenderPass(m_Device.Device(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create render pass!");
			}
    }
}
