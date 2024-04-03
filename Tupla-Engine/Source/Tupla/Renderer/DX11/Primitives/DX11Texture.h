#pragma once
#include <d3d11.h>

#include "Tupla/Renderer/Texture.h"

namespace Tupla
{
	class DX11Renderer;

	class DX11Texture : public Texture
	{
	public:
		DX11Texture(DX11Renderer* renderer);
		~DX11Texture() override;

		void SetImageData(void* imageData, u32 texWidth, u32 texHeight, u32 texBytesPerPixel) override;
		void* ImGuiRID() override { return m_TextureSRV; }

		u32 m_Width{};
		u32 m_Height{};

		ID3D11Texture2D* m_Texture{};
		ID3D11ShaderResourceView* m_TextureSRV{};

	protected:
		DX11Renderer* m_Renderer;
	};
}
