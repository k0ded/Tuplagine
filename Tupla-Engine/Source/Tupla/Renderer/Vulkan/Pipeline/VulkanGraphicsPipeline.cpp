#include "tgpch.h"
#include "VulkanGraphicsPipeline.h"

#include <fstream>

#include "Tupla/Renderer/Mesh.h"

namespace Tupla
{
    static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions {};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV)});
    
        return attributeDescriptions;
    }
    
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(
        VulkanDevice& device,
        const Ref<VulkanShader>& vertexShader,
        const Ref<VulkanShader>& fragmentShader,
        const PipelineConfigInfo& configuration): m_Device(device)
    {
        CreateGraphicsPipeline(
            vertexShader,
            fragmentShader,
            configuration
        );
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        vkDestroyShaderModule(m_Device.Device(), m_VertShaderModule, nullptr);
        vkDestroyShaderModule(m_Device.Device(), m_FragShaderModule, nullptr);
        vkDestroyPipeline(m_Device.Device(), m_GraphicsPipeline, nullptr);
    }

    void VulkanGraphicsPipeline::Bind(VkCommandBuffer commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
    }

    void VulkanGraphicsPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
    {
        // Input assembly
        configInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // Viewport info
        configInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        configInfo.ViewportInfo.viewportCount = 1;
        configInfo.ViewportInfo.pViewports = nullptr;
        configInfo.ViewportInfo.scissorCount = 1;
        configInfo.ViewportInfo.pScissors = nullptr;

        // Rasterization
        configInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.RasterizationInfo.lineWidth = 1.0f;
        configInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        configInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f; // OPTIONAL
        configInfo.RasterizationInfo.depthBiasClamp = 0.0f; // OPTIONAL
        configInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f; // OPTIONAL

        // Multisampling (Anti-aliasing)
        configInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        configInfo.MultisampleInfo.minSampleShading = 1.0f; // OPTIONAL
        configInfo.MultisampleInfo.pSampleMask = nullptr; // OPTIONAL
        configInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE; // OPTIONAL
        configInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE; // OPTIONAL

        // Color blending
        configInfo.ColorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        configInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
        configInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        configInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        configInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        configInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        configInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        configInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        configInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        configInfo.ColorBlendInfo.attachmentCount = 1;
        configInfo.ColorBlendInfo.pAttachments = &configInfo.ColorBlendAttachment;
        configInfo.ColorBlendInfo.blendConstants[0] = 0.0f; // Optional
        configInfo.ColorBlendInfo.blendConstants[1] = 0.0f; // Optional
        configInfo.ColorBlendInfo.blendConstants[2] = 0.0f; // Optional
        configInfo.ColorBlendInfo.blendConstants[3] = 0.0f; // Optional

        // Depth stencil (Depth testing)
        configInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.DepthStencilInfo.minDepthBounds = 0.0f; // Optional
        configInfo.DepthStencilInfo.maxDepthBounds = 1.0f; // Optional
        configInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.DepthStencilInfo.front = {}; // Optional
        configInfo.DepthStencilInfo.back = {}; // Optional

        // Dynamic states
        configInfo.DynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        configInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        configInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.DynamicStateEnables.size());
        configInfo.DynamicStateInfo.pDynamicStates = configInfo.DynamicStateEnables.data();
        configInfo.DynamicStateInfo.flags = 0;
    }

    void VulkanGraphicsPipeline::CreateGraphicsPipeline(
        const Ref<VulkanShader>& vertexShader,
        const Ref<VulkanShader>& fragmentShader,
        const PipelineConfigInfo& configuration)
    {
        assert(
            configuration.PipelineLayout != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: No pipeline layout provided");
        assert(
            configuration.RenderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: No render pass provided");

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertexShader->GetModule();
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragmentShader->GetModule();
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        const auto bindingDescriptions = GetBindingDescription();
        const auto attributeDescriptions = GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertextInputInfo{};
        vertextInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertextInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertextInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertextInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertextInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertextInputInfo;

        pipelineInfo.pInputAssemblyState = &configuration.InputAssemblyInfo;
        pipelineInfo.pViewportState = &configuration.ViewportInfo;
        pipelineInfo.pRasterizationState = &configuration.RasterizationInfo;
        pipelineInfo.pMultisampleState = &configuration.MultisampleInfo;
        pipelineInfo.pColorBlendState = &configuration.ColorBlendInfo;
        pipelineInfo.pDepthStencilState = &configuration.DepthStencilInfo;
        pipelineInfo.pDynamicState = &configuration.DynamicStateInfo;

        pipelineInfo.layout = configuration.PipelineLayout;
        pipelineInfo.renderPass = configuration.RenderPass;
        pipelineInfo.subpass = configuration.SubPass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_Device.Device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }
}
