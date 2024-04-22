#pragma once
#include <wrl.h>
#include <array>
#include <d3d11.h>

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11UnorderedAccessView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace Tupla
{
	enum class TextureType
	{
		Texture1D,
		Texture2D,
		Texture3D,
		TextureCube
	};

	class Texture
	{
		friend class RHI;

	public:
		u32 Width() const { return myWidth; }
		u32 Height() const { return myHeight; }
		u32 Depth() const { return myDepth; }

	private:
		std::string myName;
		u32 myBindFlags = 0;
		u32 myUsageFlags = 0;
		u32 myAccessFlags = 0;

		u32 myWidth = 1;
		u32 myHeight = 1;
		u32 myDepth = 1;

		TextureType myType;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> myTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myRTV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> myUAV;
		std::array<float, 4> myClearColor = { 0, 0, 0, 0 };

		D3D11_VIEWPORT myViewport{};
	};
}
