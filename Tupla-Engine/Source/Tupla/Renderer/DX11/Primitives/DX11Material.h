#pragma once
#include <d3d11.h>

#include "CommonUtilities/Math/Matrices/Matrix4x4.hpp"
#include "Tupla/Renderer/Material.h"

namespace Tupla
{
	class DX11Renderer;
}

namespace Tupla
{
	class DX11Material : public Material
	{
	public:
		DX11Material(DX11Renderer* renderer);

		void SetShaderStage(Ref<Shader> shader) override;
		void AttachImage(Ref<Texture> texture) override;
		void AttachBuffer(Ref<Buffer> buffer, ShaderStage stage) override;
		void BindMaterial() const override;

	private:
		DX11Renderer* m_Renderer;

		std::array<std::vector<ID3D11Buffer*>, static_cast<u64>(ShaderStage::Count)> m_Buffers;
		std::vector<Ref<Texture>> m_AttachedTextures;
		std::array<Ref<Shader>, static_cast<size_t>(ShaderStage::Count)> m_Shaders;
	};
}
