#pragma once
#include "Tupla/Renderer/Material.h"
#include "VulkanShader.h"

namespace Tupla 
{
	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial(std::string fragmentPath, std::string vertexPath);

		void SetFragmentShader(Ref<Shader> shader);
		void SetVertexShader(Ref<Shader> shader);

	protected:
		void SetInternal(u64 offset, void* src, u64 srcSize) override;
		u64 GetOffsetFromHash(u32 hash) override;

	private:
		void RecreateGraphicsPipeline();

		Ref<VulkanShader> m_FragmentShader;
		Ref<VulkanShader> m_VertexShader;
	};
}
