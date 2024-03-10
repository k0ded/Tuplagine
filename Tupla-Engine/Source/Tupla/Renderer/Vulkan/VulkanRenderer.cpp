#include "tgpch.h"
#include "VulkanRenderer.h"

#include "VulkanWindow.h"
#include "Assets/VulkanRenderingAssets.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

namespace Tupla
{
    VulkanRenderer::~VulkanRenderer()
    {
        FreeCommandBuffers();
        FreeSampler();
        ShutdownImGui();
    }

    Scope<Window> VulkanRenderer::StartWindow(const WindowProps& props)
    {
        Scope<Window> win = CreateScope<VulkanWindow>(props);
        m_Window = dynamic_cast<VulkanWindow*>(win.get());
        m_Device = CreateScope<VulkanDevice>(*m_Window);
        m_RenderingAssets = CreateScope<VulkanRenderingAssets>(*m_Device);
        
        RecreateSwapChain();
        CreateCommandBuffers();
        CreateSampler();
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

        const auto commandBuffer = m_ViewportCommandBuffer[m_CurrentFrameIndex];
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        BeginRenderPass(commandBuffer, m_ViewportPass, m_ViewportTexture[m_CurrentFrameIndex].GetFramebuffer());
    }

    void VulkanRenderer::EndFrame()
    {
        assert(m_IsFrameStarted && "Can't call EndFrame while frame is not in progress");

        const auto commandBuffer = GetCurrentCommandBuffer();
        EndRenderPass(commandBuffer);

        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer!");
        }
    
        std::vector<VkCommandBuffer> commandBuffers = { m_ViewportCommandBuffer[m_CurrentFrameIndex], commandBuffer };
        const auto result = m_SwapChain->SubmitCommandBuffers(commandBuffers.data(), &m_CurrentImage, 2);
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
        EndRenderPass(m_ViewportCommandBuffer[m_CurrentFrameIndex]);
        vkEndCommandBuffer(m_ViewportCommandBuffer[m_CurrentFrameIndex]);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(GetCurrentCommandBuffer(), &beginInfo);
        BeginRenderPass(GetCurrentCommandBuffer());

        
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Viewport");
        const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImGui::Image(m_DescriptorSet[m_CurrentFrameIndex], ImVec2{viewportPanelSize.x, viewportPanelSize.y});
        ImGui::End();
    }

    void VulkanRenderer::EndGUIFrame()
    {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), GetCurrentCommandBuffer());
    }

    void VulkanRenderer::CreateSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(m_Device->Device(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void VulkanRenderer::FreeSampler() const
    {
        vkDestroySampler(m_Device->Device(), m_TextureSampler, nullptr);
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
        
        m_DescriptorSet.resize(m_SwapChain->ImageCount());
        m_ViewportTexture.reserve(m_SwapChain->ImageCount());
        
        for (int i = 0; i < m_SwapChain->ImageCount(); ++i)
        {
            m_ViewportTexture.emplace_back(*m_Device, *m_SwapChain);
            m_DescriptorSet[i] = ImGui_ImplVulkan_AddTexture(m_TextureSampler, m_ViewportTexture[i].GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    void VulkanRenderer::ShutdownImGui() const
    {
        ImGui_ImplVulkan_DestroyFontsTexture();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(m_Device->Device(), m_ImGuiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    }

    void VulkanRenderer::BeginRenderPass(const VkCommandBuffer commandBuffer, VkRenderPass pass, VkFramebuffer fbuffer) const
    {
        assert(m_IsFrameStarted && "Can't call BeginSwapChainRenderPass if frame is not in progress");

        if(pass == VK_NULL_HANDLE)
        {
            pass = m_SwapChain->GetRenderPass();
        }

        if(fbuffer == VK_NULL_HANDLE)
        {
            fbuffer = m_SwapChain->GetFrameBuffer(m_CurrentImage);
        }
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pass;
        renderPassInfo.framebuffer = fbuffer;

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

    void VulkanRenderer::EndRenderPass(const VkCommandBuffer commandBuffer) const
    {
        assert(m_IsFrameStarted && "Can't call EndSwapChainRenderPass if frame is not in progress");
    
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
        m_ViewportCommandBuffer.resize(VulkanSwapChain::MaxFramesInFlight);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device->GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if(vkAllocateCommandBuffers(m_Device->Device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
        
        if(vkAllocateCommandBuffers(m_Device->Device(), &allocInfo, m_ViewportCommandBuffer.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void VulkanRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(m_Device->Device(), m_Device->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        vkFreeCommandBuffers(m_Device->Device(), m_Device->GetCommandPool(), static_cast<uint32_t>(m_ViewportCommandBuffer.size()), m_ViewportCommandBuffer.data());
        m_CommandBuffers.clear();
        m_ViewportCommandBuffer.clear();
    }
}
