#include "tgpch.h"
#include "DX11Material.h"

#include "DX11Shader.h"
#include "Tupla/Renderer/DX11/DX11Renderer.h"

using namespace Microsoft::WRL;

namespace Tupla
{

	DX11Material::DX11Material(DX11Renderer* renderer): m_Renderer(renderer)
	{
	}

	void DX11Material::SetShader(const Ref<Shader> shader)
	{
		const auto flags = shader->GetStages();

		if(flags == 0)
		{
			LOG_ERROR("Tried to attach a shader that hasn't compiled!");
			return;
		}

		if(flags & static_cast<int>(ShaderStage::Vertex))
		{
			m_Shaders[0] = shader;
		}

		if (flags & static_cast<int>(ShaderStage::Pixel))
		{
			m_Shaders[1] = shader;
		}

		if (flags & static_cast<int>(ShaderStage::Hull))
		{
			m_Shaders[2] = shader;
		}

		if (flags & static_cast<int>(ShaderStage::Domain))
		{
			m_Shaders[3] = shader;
		}

		if (flags & static_cast<int>(ShaderStage::Compute))
		{
			m_Shaders[4] = shader;
		}
	}

	void DX11Material::AttachImage(const Ref<Texture> texture)
	{
		m_AttachedTextures.push_back(texture);
	}

	void DX11Material::AttachBuffer(Ref<Buffer> buffer, ShaderStageSlot stage)
	{
		m_BuffersCOM[static_cast<u64>(stage)].push_back(buffer);
		m_Buffers[static_cast<u64>(stage)].push_back(buffer->GetGPUMemory().Get());
	}

	Ref<Buffer> DX11Material::GetBuffer(const size_t slot, ShaderStageSlot stage)
	{
		if(slot < globalBuffers[static_cast<u64>(stage)].size())
		{
			return globalBuffers[static_cast<u64>(stage)][slot];
		}
		return m_BuffersCOM[static_cast<u64>(stage)][slot - globalBuffers[static_cast<u64>(stage)].size()];
	}

	bool DX11Material::BindMaterial() const
	{
		if(m_Shaders[(u64)ShaderStageSlot::Vertex] == nullptr || m_Shaders[(u64)ShaderStageSlot::Pixel] == nullptr)
		{
			return false;
		}

		const auto d11vs = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStageSlot::Vertex)]);
		ComPtr<ID3D11VertexShader> vs = d11vs->GetShader<ID3D11VertexShader>();
		ComPtr<ID3D11PixelShader> ps = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStageSlot::Pixel)])->GetShader<ID3D11PixelShader>();
		const auto dcontext = m_Renderer->GetDeviceContext();

		m_Renderer->GetDeviceContext()->IASetInputLayout(d11vs->GetInputLayout().Get());

		auto& vsBuffers = m_Buffers[(u64)ShaderStageSlot::Vertex];
		dcontext->VSSetShader(vs.Get(), nullptr, 0);
		dcontext->VSSetConstantBuffers(
			(u32)globalBuffers[static_cast<u64>(ShaderStageSlot::Vertex)].size(),
			(u32)vsBuffers.size(), 
			vsBuffers.data()
		);

		auto& psBuffers = m_Buffers[(u64)ShaderStageSlot::Pixel];
		dcontext->PSSetShader(ps.Get(), nullptr, 0);
		dcontext->PSSetConstantBuffers(
			(u32)globalBuffers[static_cast<u64>(ShaderStageSlot::Pixel)].size(),
			(u32)psBuffers.size(), 
			psBuffers.data()
		);

		// Tesselation!

		auto& hsBuffers = m_Buffers[(u64)ShaderStageSlot::Hull];
		auto& dsBuffers = m_Buffers[(u64)ShaderStageSlot::Domain];
		dcontext->HSSetConstantBuffers(
			(u32)globalBuffers[static_cast<u64>(ShaderStageSlot::Hull)].size(),
			(u32)hsBuffers.size(), 
			hsBuffers.data()
		);
		dcontext->DSSetConstantBuffers(
			(u32)globalBuffers[static_cast<u64>(ShaderStageSlot::Domain)].size(),
			(u32)dsBuffers.size(), 
			dsBuffers.data()
		);

		std::vector<ID3D11ShaderResourceView*> views(m_AttachedTextures.size());

		for (int i = 0; i < views.size(); ++i)
		{
			views[i] = (ID3D11ShaderResourceView*)m_AttachedTextures[i]->ImGuiRID(); // This is the SRV
		}

		dcontext->PSSetShaderResources(0, static_cast<u32>(views.size()), views.data());
		// TODO: Sampler support?

		return true;
	}

	u64 DX11Material::GetId() const
	{
		return m_Id;
	}

	void DX11Material::SetGlobalBuffers(std::vector<Ref<Buffer>>&& aBuffers, ShaderStageSlot aSlot)
	{
		globalBuffers[static_cast<u64>(aSlot)] = aBuffers;
	}
}
