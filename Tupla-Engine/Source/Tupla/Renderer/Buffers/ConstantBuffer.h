#pragma once
#include <string>
#include <wrl.h>

#include "CommonUtilities/g_types.h"

struct ID3D11Buffer;

namespace Tupla
{
	class ConstantBuffer
	{
		friend class RHI;

		std::string myName;
		u32 mySize;
		u32 myPipelineStages;
		u32 mySlotIndex;
		void* myDataPtr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
	};
}
