#pragma once
#include <wrl.h>
#include <d3d11.h>

#include "Tupla/Renderer/Shader.h"

namespace Tupla
{
	class DX11Renderer;

	class DX11Shader : public Shader
	{
	public:
		DX11Shader(DX11Renderer* renderer);
		~DX11Shader() override;

		bool CompileShader(const std::wstring& path, ShaderStage stage, bool debug = true) override;
		bool CompileShader(void* data, size_t dataSize, ShaderStage stage) override;

		template <typename T>
		Microsoft::WRL::ComPtr<T> GetShader() = delete;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout() { return m_InputLayout; }
		
	private:
		DX11Renderer* m_Renderer;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;

		// Different compileable shader types
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GeometryShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader> m_HullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> m_DomainShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_ComputeShader;

		std::wstring m_ShaderPath;
	};

#define CONCAT_2(a, b) a##b
#define CONCAT_3(a, b, c) a##b##c
#define ADD_NAMESPACE(a, b) a::b
#define SHADER_TYPE_IMPL(x) template <> inline Microsoft::WRL::ComPtr<CONCAT_3(ID3D11, x, Shader)> DX11Shader::GetShader() { ASSERT(m_Stage & (int)ADD_NAMESPACE(ShaderStage, x), "ShaderStage mismatch! Expected:" #x) return CONCAT_3(m_, x, Shader);}

	SHADER_TYPE_IMPL(Vertex)
	SHADER_TYPE_IMPL(Pixel)
	SHADER_TYPE_IMPL(Compute)
	SHADER_TYPE_IMPL(Hull)
	SHADER_TYPE_IMPL(Domain)
}
