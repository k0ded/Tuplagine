#pragma once
#include "Tupla/Renderer/DX11/Primitives/DX11Texture.h"

namespace Tupla
{
	class DX11RenderTexture : public DX11Texture
	{
	public:
		DX11RenderTexture(DX11Renderer* renderer, u32 width, u32 height, bool usesDepth = false);
		~DX11RenderTexture() override;

		void SetImageData(void* imageData, u32 texWidth, u32 texHeight, u32 texBytesPerPixel) override {}

		void Clear(const CU::Vector4f& clearColor) const;

		ID3D11RenderTargetView* m_TextureRTV {};
		ID3D11Texture2D* m_DepthTexture {};
		ID3D11DepthStencilView* m_DepthDSV {};
		bool m_UsesDepth;
	};
}
