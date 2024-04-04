#include "tgpch.h"
#include "DX11Material.h"

#include "DX11Shader.h"
#include "Tupla/Renderer/DX11/DX11Renderer.h"

namespace Tupla
{
	DX11Material::DX11Material(DX11Renderer* renderer): m_Renderer(renderer)
	{
	}

	void DX11Material::SetShaderStage(const Ref<Shader> shader)
	{
		m_Shaders[static_cast<size_t>(shader->GetStage())] = shader;
	}

	void DX11Material::AttachImage(const Ref<Texture> texture)
	{
		m_AttachedTextures.push_back(texture);
	}

	void DX11Material::AttachBuffer(Ref<Buffer> buffer, ShaderStage stage)
	{
		m_Buffers[static_cast<u64>(stage)].push_back((ID3D11Buffer*)buffer->GetGPUMemory());
	}

	void DX11Material::BindMaterial() const
	{
		const auto d11vs = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStage::Vertex)]);
		ID3D11VertexShader* vs = d11vs->GetShader<ID3D11VertexShader>();
		ID3D11PixelShader* ps = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStage::Pixel)])->GetShader<ID3D11PixelShader>();
		const auto dcontext = m_Renderer->GetDeviceContext();

		m_Renderer->GetDeviceContext()->IASetInputLayout(d11vs->GetInputLayout());

		auto& vsBuffers = m_Buffers[(u64)ShaderStage::Vertex];
		dcontext->VSSetShader(vs, nullptr, 0);
		dcontext->VSSetConstantBuffers(0, (u32)vsBuffers.size(), vsBuffers.data());

		auto& psBuffers = m_Buffers[(u64)ShaderStage::Pixel];
		dcontext->PSSetShader(ps, nullptr, 0);
		dcontext->PSSetConstantBuffers(0, (u32)psBuffers.size(), psBuffers.data());

		// Tesselation!

		auto& hsBuffers = m_Buffers[(u64)ShaderStage::Hull];
		auto& dsBuffers = m_Buffers[(u64)ShaderStage::Domain];
		dcontext->HSSetConstantBuffers(0, (u32)hsBuffers.size(), hsBuffers.data());
		dcontext->DSSetConstantBuffers(0, (u32)dsBuffers.size(), dsBuffers.data());

		std::vector<ID3D11ShaderResourceView*> views(m_AttachedTextures.size());

		for (int i = 0; i < views.size(); ++i)
		{
			views[i] = (ID3D11ShaderResourceView*)m_AttachedTextures[i]->ImGuiRID();
		}

		dcontext->PSSetShaderResources(0, static_cast<u32>(views.size()), views.data());
		// TODO: Sampler support?
	}
}
