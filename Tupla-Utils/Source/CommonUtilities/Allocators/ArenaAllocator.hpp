#pragma once
#include <assert.h>
#include "../g_types.h"

namespace CommonUtilities
{
	template <size_t Capacity>
	class ArenaAllocator
	{
	public:
		ArenaAllocator(): myCursor(nullptr), myData(nullptr), myCommittedCapacity(0)
		{
			SYSTEM_INFO info;
			GetNativeSystemInfo(&info);
			myPageSize = info.dwPageSize;
			myGranularity = info.dwAllocationGranularity;
		}

		~ArenaAllocator()
		{
			VirtualFree(myData, 0, MEM_RELEASE);
		}

		void Reserve(u64 aCapacity)
		{
			assert(aCapacity < Capacity);

			if(aCapacity <= myCommittedCapacity) return;
			if(aCapacity % myPageSize != 0) aCapacity += myPageSize - (aCapacity % myPageSize);

			const void* pageStart = VirtualAlloc(myData + myCommittedCapacity, aCapacity - myCommittedCapacity, MEM_COMMIT, PAGE_READWRITE);
			assert(pageStart == myData + myCommittedCapacity);

			myCommittedCapacity = aCapacity;
		}

		template<typename T, typename ...Args>
		T* Allocate(Args&& ... arg)
		{
			const u64 aNewSize = (myCursor + sizeof(T)) - myData;
			assert(aNewSize < Capacity && "Arena out of memory!");
    
			if (aNewSize > myCommittedCapacity)
			{
				Reserve(aNewSize);
			}
    
			T* memory = reinterpret_cast<T*>(myData);
			::new (memory) T(std::forward<Args>(arg)...);
			myCursor += sizeof(T);
    
			return memory;
		}

		template <typename T>
		T* AllocateMany(const size_t aCount)
		{
			auto start = Allocate<T>();

			for (int i = 0; i < aCount - 1; ++i)
			{
				Allocate<T>();
			}
			
			return start;
		}

		void Pop(const u64 aBytes)
		{
			myCursor -= aBytes;

			if(myCursor < myData)
			{
				// Probably warn here
				myCursor = myData;
			}
		}

		void Clear()
		{
			myCursor = myData;
		}

		u8* Begin() const
		{
			return myData;
		}

		u8* End() const
		{
			return myCursor;
		}

	private:
		u8* myCursor;
		u8* myData;
		u64 myCommittedCapacity;

		u64 myPageSize;
		u64 myGranularity;
	};
}