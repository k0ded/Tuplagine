#pragma once
#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanWindow.h"
#include "Assets/VulkanRenderTexture.h"
#include "Tupla/Renderer/Renderer.h"

namespace Tupla
{
    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer() = default;
        ~VulkanRenderer() override;

        Scope<Window> StartWindow(const WindowProps& props) override;

        void BeginFrame() override;
        void EndFrame() override;
        void BeginGUIFrame() override;
        void EndGUIFrame() override;

        CLASS_RENDER_IMPL(VULKAN)
    private:
        void CreateSampler();
        void FreeSampler() const;
        void StartImGui();
        void ShutdownImGui() const;

        void BeginRenderPass(const VkCommandBuffer commandBuffer, VkRenderPass pass = VK_NULL_HANDLE, VkFramebuffer fbuffer = VK_NULL_HANDLE) const;
        void EndRenderPass(const VkCommandBuffer commandBuffer) const;

        void RecreateSwapChain();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        
        [[nodiscard]] VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
        [[nodiscard]] float GetAspectRatio() const { return m_SwapChain->ExtentAspectRatio(); }
        [[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameStarted; }
        [[nodiscard]] VkCommandBuffer GetCurrentCommandBuffer() const
        {
            assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress!");
            return m_CommandBuffers[m_CurrentFrameIndex];
        }

        [[nodiscard]] int GetFrameIndex() const
        {
            assert(m_IsFrameStarted && "Cannot get frame index when frame not in progress");
            return m_CurrentFrameIndex;
        }

        [[nodiscard]] bool NeedsNewPipeline() const { return m_NeedsNewPipeline; }
        void ResetNeedsNewPipeline() { m_NeedsNewPipeline = false; }
        
        VulkanWindow* m_Window;
        Scope<VulkanDevice> m_Device;
        Scope<VulkanSwapChain> m_SwapChain;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        // Viewport
        std::vector<VulkanRenderTexture> m_ViewportTexture;
        std::vector<VkCommandBuffer> m_ViewportCommandBuffer;
        VkRenderPass m_ViewportPass;
        
        VkDescriptorPool m_ImGuiPool;
        std::vector<VkDescriptorSet> m_DescriptorSet;

        VkSampler m_TextureSampler;

        uint32_t m_CurrentImage;
        int m_CurrentFrameIndex;
        bool m_IsFrameStarted {};
        bool m_NeedsNewPipeline {};
    };
}
