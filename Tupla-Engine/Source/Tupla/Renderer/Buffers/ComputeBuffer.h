#pragma once
#include "wrl.h"

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

namespace Tupla
{
	class ComputeBuffer
	{
		friend class RHI;
	private:
		u64 myDataSize;
		void* myDataPtr;

		u32 myStructuredType;

		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> myUAV;
	};
}