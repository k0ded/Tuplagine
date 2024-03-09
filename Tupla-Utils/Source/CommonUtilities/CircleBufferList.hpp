#pragma once

namespace CommonUtilities
{
	template <typename Type, size_t MaxCapacity, typename CountType = int>
	class CircleBufferList
	{
		static_assert(MaxCapacity > 0);

	public:

		CircleBufferList() : myCount{ 0 }, myCursor{ 0 } {}
		CircleBufferList(const CircleBufferList &aCircleBuffer);

		template <size_t InitializerSize>
		CircleBufferList(const Type(&aInitializerList)[InitializerSize]);

		~CircleBufferList();

		CircleBufferList &operator=(const CircleBufferList &aCircleBuffer);
		inline const Type &operator[](const CountType aIndex) const;
		inline Type &operator[](const CountType aIndex);
		inline void Add(const Type &aObject);
		inline void Clear();
		__forceinline constexpr CountType Count() const;
		__forceinline constexpr CountType Capacity() const;
		__forceinline constexpr CountType Cursor() const;
		__forceinline constexpr const Type *Data() const;
		__forceinline constexpr Type *Data();
	private:
		Type myArray[MaxCapacity];
		CountType myCount;
		CountType myCursor;
	};


#pragma warning(suppress : 26495)
	template<typename Type, size_t MaxCapacity, typename CountType>
	inline CircleBufferList<Type, MaxCapacity, CountType>::CircleBufferList(const CircleBufferList &aCircleBuffer)
	{
		myCount = aCircleBuffer.Count();
		myCursor = 0;
		if (std::is_trivially_copyable<Type>::value)
		{
			memcpy(myArray, aCircleBuffer.myArray, static_cast<size_t>(myCount) * sizeof(Type));
		}
		else
		{
			static_assert(std::is_copy_assignable<Type>::value);
			for (int i = 0; i < myCount; ++i)
			{
				myArray[i] = aCircleBuffer[i];
			}
		}
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	template<size_t InitializerSize>
	inline CircleBufferList<Type, MaxCapacity, CountType>::CircleBufferList(const Type(&aCircleBuffer)[InitializerSize])
	{
		static_assert(MaxCapacity <= std::numeric_limits<CountType>::max());
		static_assert(InitializerSize <= MaxCapacity);
		static_assert(InitializerSize <= std::numeric_limits<CountType>::max());

		myCount = static_cast<CountType>(InitializerSize);

		if (std::is_trivially_copyable<Type>::value)
		{
			memcpy(myArray, aCircleBuffer, static_cast<size_t>(myCount) * sizeof(Type));
		}
		else
		{
			static_assert(std::is_copy_assignable<Type>::value);
			for (int i = 0; i < myCount; ++i)
			{
				myArray[i] = aCircleBuffer[i];
			}
		}
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	CircleBufferList<Type, MaxCapacity, CountType>::~CircleBufferList()
	{
		myCount = 0;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline CircleBufferList<Type, MaxCapacity, CountType> &CircleBufferList<Type, MaxCapacity, CountType>::operator=(const CircleBufferList<Type, MaxCapacity, CountType> &aCircleBuffer)
	{
		if (this != &aCircleBuffer)
		{
			myCount = aCircleBuffer.myCount;

			if (std::is_trivially_copyable<Type>::value)
			{
				memcpy(myArray, aCircleBuffer.myArray, static_cast<size_t>(myCount) * sizeof(Type));
			}
			else
			{
				static_assert(std::is_copy_assignable<Type>::value);
				for (int i = 0; i < myCount; ++i)
				{
					myArray[i] = aCircleBuffer[i];
				}
			}
		}

		return *this;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline const Type &CircleBufferList<Type, MaxCapacity, CountType>::operator[](const CountType aIndex) const
	{
		assert(0 <= aIndex && aIndex < myCount);
		return myArray[aIndex];
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline Type &CircleBufferList<Type, MaxCapacity, CountType>::operator[](const CountType aIndex)
	{
		assert(0 <= aIndex && aIndex < myCount);
		return myArray[aIndex];
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void CircleBufferList<Type, MaxCapacity, CountType>::Add(const Type &aObject)
	{
		myArray[myCursor] = aObject;
		myCount = myCount < MaxCapacity ? myCount + 1 : MaxCapacity;
		myCursor = (myCursor + 1) % MaxCapacity;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline void CircleBufferList<Type, MaxCapacity, CountType>::Clear()
	{
		myCount = 0;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr CountType CircleBufferList<Type, MaxCapacity, CountType>::Count() const
	{
		return myCount;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr CountType CircleBufferList<Type, MaxCapacity, CountType>::Capacity() const
	{
		return MaxCapacity;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr CountType CircleBufferList<Type, MaxCapacity, CountType>::Cursor() const
	{
		return myCursor;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr Type *CircleBufferList<Type, MaxCapacity, CountType>::Data()
	{
		return myArray;
	}

	template<typename Type, size_t MaxCapacity, typename CountType>
	inline constexpr const Type *CircleBufferList<Type, MaxCapacity, CountType>::Data() const
	{
		return myArray;
	}
}