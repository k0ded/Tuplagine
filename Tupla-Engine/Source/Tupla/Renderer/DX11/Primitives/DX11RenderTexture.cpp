#include "tgpch.h"
#include "DX11RenderTexture.h"

#include "Tupla/Renderer/DX11/DX11Renderer.h"

Tupla::DX11RenderTexture::DX11RenderTexture(DX11Renderer* renderer, const u32 width, const u32 height, const bool usesDepth): DX11Texture(renderer)
{
	m_Width = width;
	m_Height = height;
	m_UsesDepth = usesDepth;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = m_Width;
	textureDesc.Height = m_Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // Same as framebuffer(view)
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	auto result = m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_Texture);
	ASSERT(SUCCEEDED(result), "Failed to create texture!")

	result = m_Renderer->GetDevice()->CreateShaderResourceView(m_Texture.Get(), nullptr, &m_TextureSRV);
	ASSERT(SUCCEEDED(result), "Failed to create shader resource view")

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = m_Renderer->GetDevice()->CreateRenderTargetView(m_Texture.Get(), &renderTargetViewDesc, &m_TextureRTV);
	ASSERT(SUCCEEDED(result), "Failed to create render target view")

	if (!usesDepth) return;

	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	result = m_Renderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_DepthTexture);
	ASSERT(SUCCEEDED(result), "Failed to create depth texture")

	result = m_Renderer->GetDevice()->CreateDepthStencilView(m_DepthTexture.Get(), nullptr, &m_DepthDSV);
	ASSERT(SUCCEEDED(result), "Failed to create depth stencil view")

	m_Viewport = { 0.f, 0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.f, 1.f };
}

Tupla::DX11RenderTexture::~DX11RenderTexture() = default;

void Tupla::DX11RenderTexture::Clear(const CU::Vector4f& clearColor) const
{
	m_Renderer->GetDeviceContext()->ClearRenderTargetView(m_TextureRTV.Get(), (float*)&clearColor);
	m_Renderer->GetDeviceContext()->ClearDepthStencilView(m_DepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

Tupla::Scope<Tupla::DX11Texture> Tupla::DX11RenderTexture::LinkedTexture()
{
	Scope<DX11Texture> texture = Tupla::CreateScope<DX11Texture>(m_Renderer);
	texture->m_Width = m_Width;
	texture->m_Height = m_Height;
	texture->m_Texture = m_Texture;
	texture->m_TextureSRV = m_TextureSRV;
	return texture;
}
