#pragma once
#include <vulkan/vulkan_core.h>

#include "Tupla/Renderer/Shader.h"
#include "Tupla/Renderer/SPIR-V/SpirvShader.h"
#include "Tupla/Renderer/Vulkan/VulkanDevice.h"

namespace Tupla
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
    
        VkPipelineViewportStateCreateInfo ViewportInfo;
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo RasterizationInfo;
    
        VkPipelineMultisampleStateCreateInfo MultisampleInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;

        std::vector<VkDynamicState> DynamicStateEnables;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo;
    
        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        uint32_t SubPass = 0;
    };
    
    class VulkanGraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(
            VulkanDevice& device,
            const Ref<SpirvShader>& vertexShader,
            const Ref<SpirvShader>& fragmentShader,
            const PipelineConfigInfo& configuration
            );
        ~VulkanGraphicsPipeline();

        VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
        VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
    
        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        void CreateGraphicsPipeline(
            const Ref<SpirvShader>& vertexShader,
            const Ref<SpirvShader>& fragmentShader,
            const PipelineConfigInfo& configuration
            );

        void CreateShaderModule(const Scope<std::string>& code, u32 codeSize, VkShaderModule* shaderModule) const;

        VulkanDevice& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
