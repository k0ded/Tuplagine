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

	void DX11Texture::SetImageData(void* imageData, const u32 texWidth, const u32 texHeight)
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
		textureSRD.SysMemPitch = texWidth * 4;

		auto result = m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, &textureSRD, &m_Texture);
		ASSERT(SUCCEEDED(result), "Failed to create texture!")

		result = m_Renderer->GetDevice()->CreateShaderResourceView(m_Texture, nullptr, &m_TextureSRV);
		ASSERT(SUCCEEDED(result), "Failed to create shader resource view")
	}

	// Done this way for serialization reasons. Skips multiple allocations
	// Data layout:
	// WIDTH (2 byte)
	// HEIGHT (2 byte)
	// IMAGE DATA (WIDTH * HEIGHT * BPP bytes)
	u64 DX11Texture::GetImageData(std::byte** outData)
	{
		const u64 bytes = 4 + (u64)m_Width * m_Height * BytesPerPixel;
		*outData = new std::byte[bytes];
		std::byte* data = *outData;

		ID3D11Texture2D* staging = nullptr;
		D3D11_TEXTURE2D_DESC textureDesc{};
		m_Texture->GetDesc(&textureDesc);

		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &staging);
		m_Renderer->GetDeviceContext()->CopyResource(staging, m_Texture);

		memcpy(&data[0], &m_Width, sizeof(u16));
		memcpy(&data[2], &m_Height, sizeof(u16));

		D3D11_MAPPED_SUBRESOURCE resource;
		m_Renderer->GetDeviceContext()->Map(staging, 0, D3D11_MAP_READ, 0, &resource);

		if(resource.pData)
		{
			memcpy(&data[4], resource.pData, (u64)m_Width * m_Height * BytesPerPixel);
		}

		return bytes;
	}
}
