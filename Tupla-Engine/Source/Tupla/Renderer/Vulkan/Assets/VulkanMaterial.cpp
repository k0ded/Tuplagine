#include "tgpch.h"
#include "VulkanMaterial.h"

namespace Tupla 
{
	VulkanMaterial::VulkanMaterial(std::string fragmentPath, std::string vertexPath)
	{

	}

	void VulkanMaterial::SetFragmentShader(Ref<Shader> shader)
	{
		m_FragmentShader = std::static_pointer_cast<VulkanShader>(shader);
	}

	void VulkanMaterial::SetVertexShader(Ref<Shader> shader)
	{
		m_VertexShader = std::static_pointer_cast<VulkanShader>(shader);
	}

	void VulkanMaterial::SetInternal(u64 offset, void* src, u64 srcSize)
	{

	}

	u64 VulkanMaterial::GetOffsetFromHash(u32 hash)
	{
		return u64();
	}
	void VulkanMaterial::RecreateGraphicsPipeline()
	{

	}
}
