#include "tgpch.h"
#include "VulkanSwapChain.h"

namespace Tupla
{
    VulkanSwapChain::VulkanSwapChain(VulkanDevice& deviceRef, const VkExtent2D windowExtent, const VulkanSwapChain* oldSwapChain,
                         const SwapChainPriority priority)
        : m_Device{deviceRef}, m_WindowExtent{windowExtent}
    {
        CreateSwapChain(priority, oldSwapChain);
        CreateImageViews();
        CreateRenderPass();
        CreateDepthResources();
        CreateFrameBuffers();
        CreateSyncObjects();
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        vkWaitForFences(m_Device.Device(), m_InFlightFences.size(), m_InFlightFences.data(), VK_TRUE, UINT64_MAX);

        for (const auto imageView : m_SwapChainImageViews)
        {
            vkDestroyImageView(m_Device.Device(), imageView, nullptr);
        }
        m_SwapChainImageViews.clear();

        if (m_SwapChain != nullptr)
        {
            vkDestroySwapchainKHR(m_Device.Device(), m_SwapChain, nullptr);
            m_SwapChain = nullptr;
        }

        for (int i = 0; i < m_DepthImages.size(); i++)
        {
            vkDestroyImageView(m_Device.Device(), m_DepthImageViews[i], nullptr);
            vkDestroyImage(m_Device.Device(), m_DepthImages[i], nullptr);
            vkFreeMemory(m_Device.Device(), m_DepthImageMemories[i], nullptr);
        }

        for (const auto framebuffer : m_SwapChainFrameBuffers)
        {
            vkDestroyFramebuffer(m_Device.Device(), framebuffer, nullptr);
        }

        vkDestroyRenderPass(m_Device.Device(), m_RenderPass, nullptr);

        // cleanup synchronization objects
        for (size_t i = 0; i < MaxFramesInFlight; i++)
        {
            vkDestroySemaphore(m_Device.Device(), m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device.Device(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device.Device(), m_InFlightFences[i], nullptr);
        }
    }

    VkResult VulkanSwapChain::AcquireNextImage(uint32_t* imageIndex) const
    {
        vkWaitForFences(
            m_Device.Device(),
            1,
            &m_InFlightFences[m_CurrentFrame],
            VK_TRUE,
            std::numeric_limits<uint64_t>::max());

        const VkResult result = vkAcquireNextImageKHR(
            m_Device.Device(),
            m_SwapChain,
            std::numeric_limits<uint64_t>::max(),
            m_ImageAvailableSemaphores[m_CurrentFrame], // must be a not signaled semaphore
            VK_NULL_HANDLE,
            imageIndex);

        return result;
    }

    VkResult VulkanSwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, const uint32_t* imageIndex, u32 bufferCount)
    {
        if (m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(m_Device.Device(), 1, &m_ImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_ImagesInFlight[*imageIndex] = m_InFlightFences[m_CurrentFrame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        const VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = bufferCount;
        submitInfo.pCommandBuffers = buffers;

        const VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_Device.Device(), 1, &m_InFlightFences[m_CurrentFrame]);
        if (vkQueueSubmit(m_Device.GraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        const VkSwapchainKHR swapChains[] = {m_SwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(m_Device.PresentQueue(), &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % MaxFramesInFlight;

        return result;
    }

    void VulkanSwapChain::CreateSwapChain(const SwapChainPriority priority, const VulkanSwapChain* oldSwapChain)
    {
        const SwapChainSupportDetails swapChainSupport = m_Device.GetSwapChainSupport();

        const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
        const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes, priority);
        const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

        uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.Capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Device.Surface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const QueueFamilyIndices indices = m_Device.FindPhysicalQueueFamilies();
        const uint32_t queueFamilyIndices[] = {indices.GraphicsFamily, indices.PresentFamily};

        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (oldSwapChain)
            createInfo.oldSwapchain = oldSwapChain->m_SwapChain;

        if (vkCreateSwapchainKHR(m_Device.Device(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapChainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        vkGetSwapchainImagesKHR(m_Device.Device(), m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device.Device(), m_SwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;
    }

    void VulkanSwapChain::CreateImageViews()
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_SwapChainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_SwapChainImageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device.Device(), &viewInfo, nullptr, &m_SwapChainImageViews[i]) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void VulkanSwapChain::CreateRenderPass()
    {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = GetSwapChainImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstSubpass = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        const std::array attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_Device.Device(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanSwapChain::CreateFrameBuffers()
    {
        m_SwapChainFrameBuffers.resize(ImageCount());
        for (size_t i = 0; i < ImageCount(); i++)
        {
            std::array attachments = {m_SwapChainImageViews[i], m_DepthImageViews[i]};

            const auto [width, height] = GetSwapChainExtent();
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                m_Device.Device(),
                &framebufferInfo,
                nullptr,
                &m_SwapChainFrameBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanSwapChain::CreateDepthResources()
    {
        const VkFormat depthFormat = FindDepthFormat();
        m_SwapChainDepthFormat = depthFormat;
        const auto [width, height] = GetSwapChainExtent();

        m_DepthImages.resize(ImageCount());
        m_DepthImageMemories.resize(ImageCount());
        m_DepthImageViews.resize(ImageCount());

        for (int i = 0; i < m_DepthImages.size(); i++)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            m_Device.CreateImageWithInfo(
                imageInfo,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_DepthImages[i],
                m_DepthImageMemories[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_DepthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device.Device(), &viewInfo, nullptr, &m_DepthImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void VulkanSwapChain::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MaxFramesInFlight);
        m_RenderFinishedSemaphores.resize(MaxFramesInFlight);
        m_InFlightFences.resize(MaxFramesInFlight);
        m_ImagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MaxFramesInFlight; i++)
        {
            if (vkCreateSemaphore(m_Device.Device(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(m_Device.Device(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateFence(m_Device.Device(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes,
                                                      const SwapChainPriority priority)
    {
        VkPresentModeKHR fallbackMode = VK_PRESENT_MODE_MAX_ENUM_KHR;

        switch (priority)
        {
        case SwapChainPriority::LowLatency:
            for (const auto& availablePresentMode : availablePresentModes)
            {
                // We want this
                if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    return availablePresentMode;
                }

                // This is also very good
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    fallbackMode = availablePresentMode;
                }
                else if (fallbackMode > VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    // Well id rather not but its ok
                    if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR || fallbackMode ==
                        VK_PRESENT_MODE_FIFO_RELAXED_KHR)
                    {
                        fallbackMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
                    }
                    else // Doesn't matter at this point...
                    {
                        fallbackMode = availablePresentMode;
                    }
                }
            }
            break;
        case SwapChainPriority::NoTearing:
            for (const auto& availablePresentMode : availablePresentModes)
            {
                // We want this
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    return availablePresentMode;
                }

                // This is an easy way to pick the best present mode for no tearing
                if (availablePresentMode < fallbackMode && availablePresentMode != VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    fallbackMode = availablePresentMode;
                }
            }
            break;
        case SwapChainPriority::LowPowerConsumption:
            for (const auto& availablePresentMode : availablePresentModes)
            {
                // We want this
                if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
                {
                    return availablePresentMode;
                }

                // This is also very good
                if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
                {
                    fallbackMode = availablePresentMode;
                }
                else
                {
                    // Fallback -> No tearing
                    // We want this
                    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    {
                        fallbackMode = availablePresentMode;
                    }

                    // This is an easy way to pick the best present mode for no tearing
                    if (availablePresentMode < fallbackMode && availablePresentMode != VK_PRESENT_MODE_IMMEDIATE_KHR &&
                        fallbackMode != VK_PRESENT_MODE_MAILBOX_KHR)
                    {
                        fallbackMode = availablePresentMode;
                    }
                }
            }
            break;
        }

        return fallbackMode;
    }

    VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        VkExtent2D actualExtent = m_WindowExtent;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    VkFormat VulkanSwapChain::FindDepthFormat() const
    {
        return m_Device.FindSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
}
