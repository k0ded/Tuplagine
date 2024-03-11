#pragma once
#include "Tupla/Renderer/Shader.h"
#include <Tupla/Renderer/Vulkan/VulkanDevice.h>

namespace Tupla
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(VulkanDevice& device, const std::string& filePath);
        ~VulkanShader() override;

        void CompileOnce(const std::vector<char>& code) override;
        void RecompileCode() override;

        VkShaderModule GetModule() { return m_ShaderModule; }

    private:
        VulkanDevice& m_Device;
        VkShaderModule m_ShaderModule;

        std::string m_FilePath;
    };
}
