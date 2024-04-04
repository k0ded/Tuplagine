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
		~DX11Material();

		void SetShaderStage(Ref<Shader> shader) override;
		void AttachImage(Ref<Texture> texture) override;

		void UpdateConstants(const Constants& consts) const override;
		void AttachMaterial() const override;

	private:
		DX11Renderer* m_Renderer;

		ID3D11Buffer* m_ConstantBuffer;

		std::vector<Ref<Texture>> m_AttachedTextures;
		std::array<Ref<Shader>, static_cast<size_t>(ShaderStage::Count) - 1> m_Shaders;
	};
}
