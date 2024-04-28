#pragma once
#include <string>
#include <wrl.h>

struct ID3D11Buffer;

namespace Tupla
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer(const std::string& aName, u32 aSize, u32 aPipelineStages, u32 aSlot, void* aDataPtr);

		template<typename T>
		static ConstantBuffer CreateBuffer(const std::string& aName, u32 aPipelineStages, u32 aSlot, T* aDataPtr)
		{
			return ConstantBuffer(
				aName,
				sizeof(T),
				aPipelineStages,
				aSlot,
				aDataPtr
				);
		}

		template<typename T>
		void SetData(const T& aData)
		{
			ASSERT(sizeof(aData) == mySize, "T has to be the same length as was initialized in the constant buffer");

			memcpy(&myDataPtr, &aData, sizeof(T));
		}

		void Bind() const;

	private:
		friend class RHI;

		std::string myName;
		u32 mySize;
		u32 myPipelineStages;
		u32 mySlotIndex;
		void* myDataPtr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
	};
}
