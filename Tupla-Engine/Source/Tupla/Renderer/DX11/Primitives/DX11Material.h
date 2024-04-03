#pragma once
#include <d3d11.h>

#include "Tupla/Renderer/Material.h"

namespace Tupla
{
	class DX11Material : public Material
	{
	public:
		DX11Material(ID3D11DeviceContext* deviceContext);
		void SetShader(Ref<Shader> shader) override;

		void AttachMaterial();

	private:
		ID3D11DeviceContext* m_DeviceContext;
		std::array<Ref<Shader>, static_cast<size_t>(ShaderStage::Count) - 1> m_Shaders;
	};
}
