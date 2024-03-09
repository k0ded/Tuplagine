﻿#include "tgpch.h"
#include "VulkanRenderer.h"

#include "VulkanWindow.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

namespace Tupla
{
    VulkanRenderer::~VulkanRenderer()
    {
        FreeCommandBuffers();
        ShutdownImGui();
    }

    Scope<Window> VulkanRenderer::StartWindow(const WindowProps& props)
    {
        Scope<Window> win = CreateScope<VulkanWindow>(props);
        m_Window = dynamic_cast<VulkanWindow*>(win.get());
        m_Device = CreateScope<VulkanDevice>(*m_Window);
        
        RecreateSwapChain();
        CreateCommandBuffers();
        StartImGui();
        
        return std::move(win);
    }

    void VulkanRenderer::BeginFrame()
    {
        assert(!m_IsFrameStarted && "Can't call beginFrame while already in progress");
        glfwPollEvents();
        
        const auto result = m_SwapChain->AcquireNextImage(&m_CurrentImage);

        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }
    
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire next image");
        }

        m_IsFrameStarted = true;

        const auto commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        BeginSwapChainRenderPass(commandBuffer);
    }

    void VulkanRenderer::EndFrame()
    {
        assert(m_IsFrameStarted && "Can't call EndFrame while frame is not in progress");

        const auto commandBuffer = GetCurrentCommandBuffer();
        EndSwapChainRenderPass(commandBuffer);

        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    
        const auto result = m_SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImage);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasWindowResized())
        {
            m_Window->ResetWindowResizeFlag();
            RecreateSwapChain();
        } else if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit command buffers");
        }

        m_IsFrameStarted = false;
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % VulkanSwapChain::MaxFramesInFlight;
    }

    void VulkanRenderer::BeginGUIFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanRenderer::EndGUIFrame()
    {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), GetCurrentCommandBuffer());
    }

    void VulkanRenderer::StartImGui()
    {
        //1: create descriptor pool for IMGUI
        // the size of the pool is very oversize, but it's copied from imgui demo itself.
        const VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        vkCreateDescriptorPool(m_Device->Device(), &pool_info, nullptr, &m_ImGuiPool);
        
        ImGui::CreateContext();

        //this initializes imgui for SDL
        ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(m_Window->GetWindowHandle()), true);

        //this initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_Device->Instance;
        init_info.PhysicalDevice = m_Device->GetPhysicalDevice();
        init_info.Device = m_Device->Device();
        init_info.Queue = m_Device->GraphicsQueue();
        init_info.DescriptorPool = m_ImGuiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.RenderPass = m_SwapChain->GetRenderPass();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info);

        //execute a gpu command to upload imgui font textures
        ImGui_ImplVulkan_CreateFontsTexture();
    }

    void VulkanRenderer::ShutdownImGui() const
    {
        ImGui_ImplVulkan_DestroyFontsTexture();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(m_Device->Device(), m_ImGuiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    }

    void VulkanRenderer::BeginSwapChainRenderPass(const VkCommandBuffer commandBuffer) const
    {
        assert(m_IsFrameStarted && "Can't call BeginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_CurrentImage);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        const VkRect2D scissor{{0, 0}, m_SwapChain->GetSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanRenderer::EndSwapChainRenderPass(const VkCommandBuffer commandBuffer) const
    {
        assert(m_IsFrameStarted && "Can't call EndSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
    
        vkCmdEndRenderPass(commandBuffer);
    }

    void VulkanRenderer::RecreateSwapChain()
    {
        auto extent = m_Window->GetExtents();
        while (extent.x == 0  || extent.y == 0)
        {
            extent = m_Window->GetExtents();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_Device->Device());
        const std::shared_ptr oldChain = std::move(m_SwapChain);
        m_SwapChain = CreateScope<VulkanSwapChain>(*m_Device, m_Window->GetExtents(), oldChain.get());

        if(oldChain && oldChain->CompareSwapFormats(*m_SwapChain))
        {
            m_NeedsNewPipeline = true;
        }
    }

    void VulkanRenderer::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(VulkanSwapChain::MaxFramesInFlight);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device->GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if(vkAllocateCommandBuffers(m_Device->Device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void VulkanRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(m_Device->Device(), m_Device->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.clear();
    }
}
