#include "tgpch.h"
#include "DX11Shader.h"

#include <d3dcompiler.h>
#include <map>

#include "Tupla/Renderer/DX11/DX11Renderer.h"

namespace Tupla
{
	std::map<ShaderStage, std::string> entryPointNames = {
		{ShaderStage::Compute, "comp"},
		{ShaderStage::Vertex, "vert"},
		{ShaderStage::Pixel, "frag"},
		{ShaderStage::Hull, "hull"},
		{ShaderStage::Domain, "domain"}
	};

	std::map<ShaderStage, std::string> targets = {
		{ShaderStage::Compute, "cs_5_0"},
		{ShaderStage::Vertex, "vs_5_0"},
		{ShaderStage::Pixel, "ps_5_0"},
		{ShaderStage::Hull, "hs_5_0"},
		{ShaderStage::Domain, "ds_5_0"}
	};

	DX11Shader::DX11Shader(DX11Renderer* renderer, const std::wstring& path, const ShaderStage stage)
	{
		m_Renderer = renderer;
		m_ShaderPath = path;
		m_Stage = stage;
		DX11Shader::CompileShader();
	}

	DX11Shader::~DX11Shader()
	{
		switch (m_Stage)
		{
		case ShaderStage::Vertex:
			m_VertexShader->Release();
			break;
		case ShaderStage::Hull:
			m_HullShader->Release();
			break;
		case ShaderStage::Domain:
			m_DomainShader->Release();
			break;
		case ShaderStage::Pixel:
			m_PixelShader->Release();
			break;
		case ShaderStage::Compute:
			m_ComputeShader->Release();
			break;
		}
	}

	void DX11Shader::CompileShader()
	{
		ASSERT(m_Stage != ShaderStage::None, "Cannot compile a shader that doesnt have a stage!");

		ID3DBlob* cso;
		auto result = D3DCompileFromFile(
			m_ShaderPath.c_str(), 
			nullptr, nullptr, 
			entryPointNames[m_Stage].c_str(), 
			targets[m_Stage].c_str(),
			0, 0, 
			&cso, 
			nullptr
		);

		ASSERT(SUCCEEDED(result), "Failed to compile shader");

		switch (m_Stage)
		{
		case ShaderStage::Vertex:
			result = m_Renderer->GetDevice()->CreateVertexShader(cso->GetBufferPointer(), cso->GetBufferSize(), nullptr, &m_VertexShader);
			ASSERT(SUCCEEDED(result), "Failed to create shader from compiled source");
			{
				D3D11_INPUT_ELEMENT_DESC ieDesc[] =
				{
					{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 position
					{ "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 normal
					{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float2 texcoord
					{ "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // float3 color
				};


				result = m_Renderer->GetDevice()->CreateInputLayout(ieDesc, ARRAYSIZE(ieDesc), cso->GetBufferPointer(), cso->GetBufferSize(), &m_InputLayout);

				ASSERT(SUCCEEDED(result), "Failed to create input layout for vertex shader");
			}
			
			break;
		case ShaderStage::Hull:
			result = m_Renderer->GetDevice()->CreateHullShader(cso->GetBufferPointer(), cso->GetBufferSize(), nullptr, &m_HullShader);
			ASSERT(SUCCEEDED(result), "Failed to create shader from compiled source")
			break;
		case ShaderStage::Domain:
			result = m_Renderer->GetDevice()->CreateDomainShader(cso->GetBufferPointer(), cso->GetBufferSize(), nullptr, &m_DomainShader);
			ASSERT(SUCCEEDED(result), "Failed to create shader from compiled source")
			break;
		case ShaderStage::Pixel:
			result = m_Renderer->GetDevice()->CreatePixelShader(cso->GetBufferPointer(), cso->GetBufferSize(), nullptr, &m_PixelShader);
			ASSERT(SUCCEEDED(result), "Failed to create shader from compiled source")
			break;
		case ShaderStage::Compute:
			result = m_Renderer->GetDevice()->CreateComputeShader(cso->GetBufferPointer(), cso->GetBufferSize(), nullptr, &m_ComputeShader);
			ASSERT(SUCCEEDED(result), "Failed to create shader from compiled source")
			break;
		}
	}
}
