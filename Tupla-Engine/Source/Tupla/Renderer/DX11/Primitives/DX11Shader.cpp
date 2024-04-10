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

	DX11Shader::DX11Shader(DX11Renderer* renderer)
	{
		m_Renderer = renderer;
	}

	DX11Shader::~DX11Shader() = default;

	bool DX11Shader::CompileShader(const std::wstring& path, const ShaderStage stage, bool debug, const std::string& debugName)
	{
		if (stage == ShaderStage::None)
		{
			LOG_ERROR("Cannot compile a shader that doesnt have a stage!");
			return false;
		}

		ID3DBlob* cso;
		ID3DBlob* errors;

#ifdef DEBUG
		auto flags = debug ? D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : D3DCOMPILE_OPTIMIZATION_LEVEL1;
#else
		auto flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		const auto result = D3DCompileFromFile(
			path.c_str(),
			nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPointNames[stage].c_str(),
			targets[stage].c_str(),
			flags, 0,
			&cso,
			&errors
		);

		if (result == ERROR_FILE_NOT_FOUND)
		{
			LOG_ERROR("Couldn't find shader!");
			return false;
		}

		if (FAILED(result))
		{
			std::string errorStr;
			errorStr.reserve(errors->GetBufferSize());

			for (size_t i = 0; i < errors->GetBufferSize(); ++i)
			{
				errorStr.push_back(*(static_cast<char*>(errors->GetBufferPointer()) + i));
			}

			LOG_ERROR("Failed to compile shader: {}", errorStr);

			return false;
		}

		return CompileShader(cso->GetBufferPointer(), cso->GetBufferSize(), stage, debugName);
	}

	bool DX11Shader::CompileShader(void* data, size_t dataSize, ShaderStage stage, const std::string& debugName)
	{
		HRESULT result;
		switch (stage)
		{
		case ShaderStage::Vertex:
			result = m_Renderer->GetDevice()->CreateVertexShader(data, dataSize, nullptr, &m_VertexShader);

			if (FAILED(result)) break;
			DX11Renderer::SetObjectName(m_VertexShader.Get(), (debugName + "_VS").c_str());

			result = m_Renderer->GetDevice()->CreateInputLayout(
				Vertex::InputDescription,
				ARRAYSIZE(Vertex::InputDescription),
				data,
				dataSize,
				&m_InputLayout
			);

			if (FAILED(result))
			{
				LOG_ERROR("Failed to bind input layout!");
				return false;
			}
			DX11Renderer::SetObjectName(m_InputLayout.Get(), (debugName + "_VInputLayout").c_str());
			break;
		case ShaderStage::Hull:
			result = m_Renderer->GetDevice()->CreateHullShader(data, dataSize, nullptr, &m_HullShader);
			DX11Renderer::SetObjectName(m_HullShader.Get(), (debugName + "_HS").c_str());
			break;
		case ShaderStage::Domain:
			result = m_Renderer->GetDevice()->CreateDomainShader(data, dataSize, nullptr, &m_DomainShader);
			DX11Renderer::SetObjectName(m_DomainShader.Get(), (debugName + "_DS").c_str());
			break;
		case ShaderStage::Pixel:
			result = m_Renderer->GetDevice()->CreatePixelShader(data, dataSize, nullptr, &m_PixelShader);
			DX11Renderer::SetObjectName(m_PixelShader.Get(), (debugName + "_PS").c_str());
			break;
		case ShaderStage::Compute:
			result = m_Renderer->GetDevice()->CreateComputeShader(data, dataSize, nullptr, &m_ComputeShader);
			DX11Renderer::SetObjectName(m_ComputeShader.Get(), (debugName + "_CS").c_str());
			break;
		}

		m_Stage |= static_cast<int>(stage);

		if (FAILED(result))
		{
			LOG_ERROR("Failed to create shader from compiled source");
			return false;
		}

		return true;
	}
}
