#pragma once
#include <d3dcommon.h>
#include <wrl.h>

struct ID3D11DeviceChild;
struct _D3D_SHADER_MACRO;
struct IUnknown;

namespace Tupla
{
	enum class ShaderType : unsigned
	{
		PixelShader,
		VertexShader,
		GeometryShader,
		DomainShader,
		HullShader,
		ComputeShader,
		Unknown
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

		bool CompileShader(const std::string& aPath, ShaderType aType, std::vector<ShaderMacro> aMacros = {}, bool aDebug = false, Microsoft::WRL::ComPtr<ID3DBlob>* outShaderCode = nullptr);

		const Microsoft::WRL::ComPtr<ID3D11DeviceChild>& GetShader() const { return myShader; }
		FORCEINLINE ShaderType GetShaderType() const { return myType; }

	private:
		std::string myName;

		ShaderType myType = ShaderType::Unknown;
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> myShader;
	};
}