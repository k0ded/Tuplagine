#include "tgpch.h"
#include "DX11Texture.h"

#include "Tupla/Renderer/DX11/DX11Renderer.h"

namespace Tupla
{
	DX11Texture::DX11Texture(DX11Renderer* renderer)
	{
		m_Renderer = renderer;
	}

	DX11Texture::~DX11Texture()
	{
		m_Texture->Release();
		m_TextureSRV->Release();
	}

	void DX11Texture::SetImageData(void* imageData, const u32 texWidth, const u32 texHeight, const u32 texBytesPerPixel)
	{
		m_Width = texWidth;
		m_Height = texHeight;

		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = texWidth;
		textureDesc.Height = texHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // Same as framebuffer(view)
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA textureSRD{};
		textureSRD.pSysMem = imageData;
		textureSRD.SysMemPitch = texWidth * texBytesPerPixel;

		auto result = m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, &textureSRD, &m_Texture);
		ASSERT(SUCCEEDED(result), "Failed to create texture!")

		result = m_Renderer->GetDevice()->CreateShaderResourceView(m_Texture, nullptr, &m_TextureSRV);
		ASSERT(SUCCEEDED(result), "Failed to create shader resource view")
	}
}
