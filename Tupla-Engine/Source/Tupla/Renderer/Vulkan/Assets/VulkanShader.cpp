#include "tgpch.h"
#include "VulkanShader.h"
#include "Tupla/Core/Application.h"
#include <CommonUtilities/File.h>

namespace Tupla
{
	VulkanShader::VulkanShader(VulkanDevice& device, const std::string& filePath): m_Device(device)
	{
		m_FilePath = filePath;
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device.Device(), m_ShaderModule, nullptr);
	}

	void VulkanShader::CompileOnce(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device.Device(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	}

	void VulkanShader::RecompileCode()
	{
		if (m_FilePath.empty()) return;

		std::vector<char> buffer;
		Application::Get().GetAssetManager().ReadBinaryFromFile(m_FilePath, buffer);
		CompileOnce(buffer);
	}
}
