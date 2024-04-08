#pragma once
#include "Tupla/Renderer/DX11/Primitives/DX11Texture.h"

namespace Tupla
{
	class DX11RenderTexture : public DX11Texture
	{
	public:
		DX11RenderTexture(DX11Renderer* renderer, u32 width, u32 height, bool usesDepth = false);
		~DX11RenderTexture() override;

		bool SetImageData(void* imageData, u32 texWidth, u32 texHeight) override { return false; }

		void Clear(const CU::Vector4f& clearColor) const;

		// Can be used to remove render texture components of a texture without copying it.
		// If you'd want to make thumbnails you might have a render texture to generate the thumbnail
		// But then you would demote it to a texture with this method. And finally you'd call the render texture destructor
		// NOTE: This does not copy the texture, it merely makes a reference to it.
		Scope<DX11Texture> LinkedTexture();

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_TextureRTV {};
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthTexture {};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthDSV {};
		D3D11_VIEWPORT m_Viewport {};
		bool m_UsesDepth;
	};
}
