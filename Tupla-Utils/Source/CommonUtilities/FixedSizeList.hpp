#pragma once
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <limits>
#include <initializer_list>
#include <type_traits>

namespace CommonUtilities
{
	template <typename Type, size_t MaxCapacity, typename CountType = int>
		class FixedSizeList
	{
	public:
		FixedSizeList();
		FixedSizeList(const FixedSizeList &aVectorOnStack);
		template <size_t InitializerSize>
		FixedSizeList(const Type(&aInitializerList)[InitializerSize]);
		~FixedSizeList();
		FixedSizeList &operator=(const FixedSizeList &aVectorOnStack);
		inline const Type &operator[](const CountType aIndex) const;
		inline Type &operator[](const CountType aIndex);
		inline void Add(const Type &aObject);
		inline void Insert(const CountType aIndex, const Type &aObject);
		inline void RemoveCyclic(const Type &aObject);
		inline void RemoveCyclicAtIndex(const CountType aIndex);
		inline void RemoveAtIndex(const CountType aIndex);
		inline void Clear();
		__forceinline constexpr CountType Count() const;
		__forceinline constexpr CountType Capacity() const;
		__forceinline constexpr const Type *Data() const;
		__forceinline constexpr Type *Data();
	private:
		Type myArray[MaxCapacity];
		CountType myCount = 0;
	};


#pragma warning(suppress : 26495)
	template<typename Type, size_t MaxCapacity, typename CountType>
	inline FixedSizeList<Type, MaxCapacity, CountType>::FixedSizeList()
		: myCount{ 0 }
	{
		static_assert(MaxCapacity <= (std::numeric_limits<CountType>::max)());
	}


#pragma warning(suppress : 26495)
	template<typename Type, size_t MaxCapacity, typename CountType>
	inline FixedSizeList<Type, MaxCapacity, CountType>::FixedSizeList(const FixedSizeList &aVectorOnStack)
	{
		myCount = aVectorOnStack.Count();

		if (std::is_trivially_copyable_v<Type>)
		{
			memcpy(myArray, aVectorOnStack.myArray, static_cast<size_t>(myCount) * sizeof(Type));
		}
		else
		{
			for (int i = 0; i < myCount; ++i)
			{
				myArray[i] = aVectorOnStack[i];
			}
		}
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	template<size_t InitializerSize>
	inline FixedSizeList<Type, MaxCapacity, CountType>::FixedSizeList(const Type(&aInitializerList)[InitializerSize])
	{
		static_assert(MaxCapacity <= (std::numeric_limits<CountType>::max)());
		static_assert(InitializerSize <= MaxCapacity);
		static_assert(InitializerSize <= (std::numeric_limits<CountType>::max)());

		myCount = static_cast<CountType>(InitializerSize);


		if (std::is_trivially_copyable_v<Type>)
		{
			memcpy(myArray, &aInitializerList, InitializerSize * sizeof(Type));
		}
		else
		{
			for (int i = 0; i < myCount; ++i)
			{
				myArray[i] = aInitializerList[i];
			}
		}
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	FixedSizeList<Type, MaxCapacity, CountType>::~FixedSizeList()
	{
		myCount = 0;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline FixedSizeList<Type, MaxCapacity, CountType> &FixedSizeList<Type, MaxCapacity, CountType>::operator=(const FixedSizeList<Type, MaxCapacity, CountType> &aVectorOnStack)
	{
		if (this != &aVectorOnStack)
		{
			myCount = aVectorOnStack.myCount;
			if (std::is_trivially_copyable_v<Type>)
			{
				memcpy(myArray, aVectorOnStack.myArray, myCount * sizeof(Type));
			}
			else
			{
				for (int i = 0; i < myCount; ++i)
				{
					myArray[i] = aVectorOnStack.myArray[i];
				}
			}
		}

		return *this;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline const Type &FixedSizeList<Type, MaxCapacity, CountType>::operator[](const CountType aIndex) const
	{
		assert(0 <= aIndex && aIndex < myCount);
		return myArray[aIndex];
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline Type &FixedSizeList<Type, MaxCapacity, CountType>::operator[](const CountType aIndex)
	{
		assert(0 <= aIndex && aIndex < myCount);
		return myArray[aIndex];
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void FixedSizeList<Type, MaxCapacity, CountType>::Add(const Type &aObject)
	{
		assert(myCount < MaxCapacity);

		myArray[myCount] = aObject;
		myCount += 1;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void FixedSizeList<Type, MaxCapacity, CountType>::Insert(const CountType aIndex, const Type &aObject)
	{
		assert(myCount < MaxCapacity && aIndex >= 0 && aIndex <= myCount);
		if (aIndex == myCount)
		{
			Add(aObject);
		}
		else
		{
			if (std::is_trivially_copyable_v<Type>) // TODO: Not sure this is the correct C++ whatever attribute to use here?
			{
				memmove(myArray + aIndex + 1, myArray + aIndex, sizeof(Type));

				myArray[aIndex] = aObject;
				myCount += 1;
			}
			else
			{
				CountType cursor = myCount;
				while (cursor > aIndex)
				{
					myArray[cursor] = myArray[cursor - 1];
					--cursor;
				}

				myArray[aIndex] = aObject;
				myCount += 1;
			}
		}

	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void FixedSizeList<Type, MaxCapacity, CountType>::RemoveCyclic(const Type &aObject)
	{
		for (CountType i = 0; i < myCount; ++i)
		{
			if (myArray[i] == aObject)
			{
				RemoveCyclicAtIndex(i);
				return;
			}
		}
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void FixedSizeList<Type, MaxCapacity, CountType>::RemoveCyclicAtIndex(const CountType aIndex)
	{
		assert(0 <= aIndex && aIndex < myCount);

		myArray[aIndex] = myArray[myCount - 1];
		myCount -= 1;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void FixedSizeList<Type, MaxCapacity, CountType>::RemoveAtIndex(const CountType aIndex)
	{
		assert(0 <= aIndex && aIndex < myCount);
		if (aIndex != myCount)
		{
			std::memmove(&myArray[aIndex], &myArray[aIndex + 1], sizeof(Type) * (myCount - aIndex - 1));
		}

		myCount -= 1;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void FixedSizeList<Type, MaxCapacity, CountType>::Clear()
	{
		myCount = 0;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr CountType FixedSizeList<Type, MaxCapacity, CountType>::Count() const
	{
		return myCount;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr CountType FixedSizeList<Type, MaxCapacity, CountType>::Capacity() const
	{
		return MaxCapacity;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr Type *FixedSizeList<Type, MaxCapacity, CountType>::Data()
	{
		return myArray;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr const Type *FixedSizeList<Type, MaxCapacity, CountType>::Data() const
	{
		return myArray;
	}
}

namespace CU = CommonUtilities;