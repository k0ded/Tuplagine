#include "tgpch.h"
#include "DX11Material.h"

#include "DX11Shader.h"

namespace Tupla
{
	DX11Material::DX11Material(ID3D11DeviceContext* deviceContext): m_DeviceContext(deviceContext)
	{
	}

	void DX11Material::SetShader(Ref<Shader> shader)
	{
		m_Shaders[static_cast<size_t>(shader->GetStage())] = shader;
	}

	void DX11Material::AttachMaterial()
	{
		ID3D11VertexShader* vs = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStage::Vertex)])->GetShader<ID3D11VertexShader>();
		ID3D11PixelShader* ps = std::static_pointer_cast<DX11Shader>(m_Shaders[static_cast<u64>(ShaderStage::Pixel)])->GetShader<ID3D11PixelShader>();
		m_DeviceContext->VSSetShader(vs, nullptr, 0);


		m_DeviceContext->PSSetShader(ps, nullptr, 0);
	}
}
