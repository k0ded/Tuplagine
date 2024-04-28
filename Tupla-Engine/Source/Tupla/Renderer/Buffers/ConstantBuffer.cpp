#include "tgpch.h"
#include "ConstantBuffer.h"

#include "Tupla/Renderer/RHI/RHI.h"

namespace Tupla
{
	ConstantBuffer::ConstantBuffer(const std::string& aName, u32 aSize, u32 aPipelineStages, u32 aSlot, void* aDataPtr)
	{
		myName = aName;
		mySize = aSize;
		myPipelineStages = aPipelineStages;
		mySlotIndex = aSlot;
		myDataPtr = aDataPtr;
		myBuffer = nullptr;

		RHI::CreateConstantBuffer(myBuffer, (aSize + 0xF) & 0xFFFFFFF0, aName);
	}

	void ConstantBuffer::Bind() const
	{
		RHI::SetConstantBuffer(myPipelineStages, mySlotIndex, *this);
	}
}
