#include "tgpch.h"
#include "Shader.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../RHI/RHI.h"
#include "CommonUtilities/File.h"

std::unordered_map<Tupla::ShaderType, std::string> TypeToEntry = {
	{ Tupla::ShaderType::PixelShader, "frag" },
	{ Tupla::ShaderType::VertexShader, "vert" },
	{ Tupla::ShaderType::GeometryShader, "geom" },
	{ Tupla::ShaderType::ComputeShader, "comp" },
};

std::unordered_map<Tupla::ShaderType, std::string> TypeToTarget = {
	{ Tupla::ShaderType::PixelShader, "ps_5_0" },
	{ Tupla::ShaderType::VertexShader, "vs_5_0" },
	{ Tupla::ShaderType::GeometryShader, "gs_5_0" },
	{ Tupla::ShaderType::ComputeShader, "cs_5_0" },
};

Tupla::Shader::Shader() = default;

Tupla::Shader::~Shader() = default;

bool Tupla::Shader::CompileShader(const std::string& aPath, ShaderType aType, std::vector<ShaderMacro> aMacros, bool aDebug)
{
	const D3D_SHADER_MACRO* macroPtr = nullptr; // So we get a nullptr if we haven't defined any macros!
	if(!aMacros.empty())
	{
		// Null terminates the macros
		aMacros.push_back({ NULL, NULL });
		macroPtr = reinterpret_cast<D3D_SHADER_MACRO*>(aMacros.data()); // ShaderMacro has the same definition, just a different name!
	}

	u32 flags = 0;

	if(aDebug)
	{
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
	}
	else
	{
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
	}

	std::string output;
	CU::ReadFile(aPath.c_str(), output);

	ComPtr<ID3DBlob> compiledBlob;
	ComPtr<ID3DBlob> errorBlob;
	const HRESULT result = D3DCompile(
		output.data(),
		output.size(),
		aPath.c_str(),
		macroPtr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		TypeToEntry.at(aType).c_str(),
		TypeToTarget.at(aType).c_str(),
		flags,
		0,
		compiledBlob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

	if(FAILED(result))
	{
		if(errorBlob)
		{
			LOG_ERROR(errorBlob->GetBufferPointer());
		}

		return false;
	}

	return RHI::LoadShaderFromMemory(this, aPath, compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize());
}
