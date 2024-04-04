#include "tgpch.h"
#include "DX11Material.h"

#include "DX11Shader.h"
#include "Tupla/Renderer/DX11/DX11Renderer.h"

namespace Tupla
{
	DX11Material::DX11Material(DX11Renderer* renderer): m_Renderer(renderer)
	{
		D3D11_BUFFER_DESC constantBufferDesc{};
		constantBufferDesc.ByteWidth = sizeof(Constants) + 0xf & 0xfffffff0; // ensure buffer size is multiple of 16 bytes
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated every frame
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		const auto result = renderer->GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffer);
		ASSERT(SUCCEEDED(result), "Failed to create constant buffer");
	}

	DX11Material::~DX11Material()
	{
		m_ConstantBuffer->Release();
	}

	void DX11Material::SetShaderStage(const Ref<Shader> shader)
	{
		m_Shaders[static_cast<size_t>(shader->GetStage())] = shader;
	}

	void DX11Material::AttachImage(const Ref<Texture> texture)
	{
		m_AttachedTextures.push_back(texture);
	}

	void DX11Material::UpdateConstants(const Constants& consts) const
	{
		D3D11_MAPPED_SUBRESOURCE constants;

		const auto result = m_Renderer->GetDeviceContext()->Map(m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constants);
		ASSERT(SUCCEEDED(result), "Failed to map constant buffer");
		{
			*(Constants*)constants.pData = consts;
		}
		m_Renderer->GetDeviceContext()->Unmap(m_ConstantBuffer, 0);
	}

	void DX11Material::AttachMaterial() const
	{
		auto d11vs = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStage::Vertex)]);
		ID3D11VertexShader* vs = d11vs->GetShader<ID3D11VertexShader>();
		ID3D11PixelShader* ps = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStage::Pixel)])->GetShader<ID3D11PixelShader>();
		const auto dcontext = m_Renderer->GetDeviceContext();

		m_Renderer->GetDeviceContext()->IASetInputLayout(d11vs->GetInputLayout());

		dcontext->VSSetShader(vs, nullptr, 0);
		dcontext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
		// TODO: Additional buffers support?

		dcontext->PSSetShader(ps, nullptr, 0);

		std::vector<ID3D11ShaderResourceView*> views(m_AttachedTextures.size());

		for (int i = 0; i < views.size(); ++i)
		{
			views[i] = (ID3D11ShaderResourceView*)m_AttachedTextures[i]->ImGuiRID();
		}

		dcontext->PSSetShaderResources(0, static_cast<u32>(views.size()), views.data());
		// TODO: Sampler support?
	}
}
