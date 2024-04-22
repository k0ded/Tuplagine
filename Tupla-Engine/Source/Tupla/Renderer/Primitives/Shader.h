#pragma once
#include <wrl.h>

struct ID3D11DeviceChild;

namespace Tupla
{
	enum class ShaderType : unsigned
	{
		Unknown,
		VertexShader,
		GeometryShader,
		PixelShader,
		ComputeShader
	};

	class Shader
	{
		friend class RHI;

	public:
		Shader();
		~Shader();

		const Microsoft::WRL::ComPtr<ID3D11DeviceChild>& GetShader() const { return myShader; }
		FORCEINLINE ShaderType GetShaderType() const { return myType; }

	private:
		std::string myName;

		ShaderType myType = ShaderType::Unknown;
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> myShader;
	};
}