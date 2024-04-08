#pragma once
#include <d3d11.h>

#include "DX11Buffer.h"
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

		void SetShader(Ref<Shader> shader) override;
		void AttachImage(Ref<Texture> texture) override;
		void AttachBuffer(Ref<Buffer> buffer, ShaderStageSlot stage) override;
		Ref<Buffer> GetBuffer(size_t slot, ShaderStageSlot stage) override;
		bool BindMaterial() const override;

	private:
		DX11Renderer* m_Renderer;

		std::array<std::vector<Ref<Buffer>>, (u64)ShaderStageSlot::Count> m_BuffersCOM;
		std::array<std::vector<ID3D11Buffer*>, (u64)ShaderStageSlot::Count> m_Buffers; // MANAGED BY ComPtr in DX11Buffer above!
		std::vector<Ref<Texture>> m_AttachedTextures;
		std::array<Ref<Shader>, (u64)ShaderStageSlot::Count> m_Shaders;
	};
}
