#pragma once
#include <d3d11.h>

#include "Tupla/Renderer/Shader.h"

namespace Tupla
{
	class DX11Renderer;

	class DX11Shader : public Shader
	{
	public:
		DX11Shader(DX11Renderer* renderer, const std::wstring& path, ShaderStage stage);
		~DX11Shader() override;

		void CompileShader() override;

		template <typename T>
		T* GetShader() = delete;
		ID3D11InputLayout* GetInputLayout() const { return m_InputLayout; }
		
	private:
		DX11Renderer* m_Renderer;
		ID3D11InputLayout* m_InputLayout;

		union
		{
			ID3D11VertexShader* m_VertexShader;
			ID3D11GeometryShader* m_GeometryShader;
			ID3D11HullShader* m_HullShader;
			ID3D11DomainShader* m_DomainShader;
			ID3D11PixelShader* m_PixelShader;
			ID3D11ComputeShader* m_ComputeShader;
		};

		std::wstring m_ShaderPath;
	};

#define CONCAT_2(a, b) a##b
#define CONCAT_3(a, b, c) a##b##c
#define ADD_NAMESPACE(a, b) a::b
#define SHADER_TYPE_IMPL(x) template <> inline CONCAT_3(ID3D11, x, Shader*) DX11Shader::GetShader() { ASSERT(m_Stage == ADD_NAMESPACE(ShaderStage, x), "ShaderStage mismatch! Expected:" #x) return CONCAT_3(m_, x, Shader);}

	SHADER_TYPE_IMPL(Vertex)
	SHADER_TYPE_IMPL(Pixel)
	SHADER_TYPE_IMPL(Compute)
	SHADER_TYPE_IMPL(Hull)
	SHADER_TYPE_IMPL(Domain)
}
