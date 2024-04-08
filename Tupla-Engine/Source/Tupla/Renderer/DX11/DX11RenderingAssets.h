#pragma once
#include "Tupla/Renderer/RenderingAssets.h"

namespace Tupla
{
	class DX11RenderingAssets : public RenderingAssets
	{
	public:
		Scope<Material> CreateMaterial() override;
		Scope<Shader> CreateShader() override;
		Scope<Texture> CreateTexture() override;
		Scope<Mesh> CreateMesh() override;
		Scope<Buffer> CreateBuffer(u32 byteSize) override;
	};
}
