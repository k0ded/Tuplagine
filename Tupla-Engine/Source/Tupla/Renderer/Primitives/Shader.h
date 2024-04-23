#pragma once
#include <wrl.h>

struct ID3D11DeviceChild;
struct _D3D_SHADER_MACRO;

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

	struct ShaderMacro
	{
		const char* Name;
		const char* Definition;
	};

	class Shader
	{
		friend class RHI;

	public:
		Shader();
		~Shader();

		bool CompileShader(const std::string& aPath, std::vector<ShaderMacro> aMacros, ShaderType aType, bool aDebug = false);

		const Microsoft::WRL::ComPtr<ID3D11DeviceChild>& GetShader() const { return myShader; }
		FORCEINLINE ShaderType GetShaderType() const { return myType; }

	private:
		std::string myName;

		ShaderType myType = ShaderType::Unknown;
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> myShader;
	};
}