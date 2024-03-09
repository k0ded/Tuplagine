#pragma once
#include <string.h>
#include <type_traits>
#include <vector>

#include 

namespace CommonUtilities
{
	template <typename Type>
		class List
	{
	public:
		List();
		List(const List &aVectorOnStack);
		List(const std::initializer_list<Type>& aInitializerList);
		~List() = default;
		List &operator=(const List &aVectorOnStack);
		inline const Type &operator[](const size_t aIndex) const;
		inline Type &operator[](const size_t aIndex);
		inline void Add(const Type &aObject);
		inline void Insert(const size_t aIndex, const Type &aObject);
		inline void RemoveCyclic(const Type &aObject);
		inline void RemoveCyclicAtIndex(const size_t aIndex);
		inline void RemoveAtIndex(const size_t aIndex);
		inline void Clear();
		__forceinline constexpr size_t Count() const;
		__forceinline constexpr size_t Capacity() const;
		__forceinline constexpr const Type *Data() const;
		__forceinline constexpr Type *Data();
		Type& back();

	private:
		std::vector<Type> myVector;
	};


#pragma warning(suppress : 26495)
	template<typename Type>
	inline List<Type>::List()
	{
	}


#pragma warning(suppress : 26495)
	template<typename Type>
	inline List<Type>::List(const List &aVectorOnStack)
	{
		aVectorOnStack.myVector = myVector;
	}

	template<typename Type>
	inline List<Type>::List(const std::initializer_list<Type>& aInitializerList)
	{
		myVector = aInitializerList;
	}

	template<typename Type>
	inline List<Type> &List<Type>::operator=(const List &aVectorOnStack)
	{
		myVector = aVectorOnStack.myVector;
		return *this;
	}

	template<typename Type>
	inline const Type &List<Type>::operator[](const size_t aIndex) const
	{
		return myVector.at(aIndex);
	}

	template<typename Type>
	inline Type &List<Type>::operator[](const size_t aIndex)
	{
		return myVector.at(aIndex);
	}

	template<typename Type>
	inline void List<Type>::Add(const Type &aObject)
	{
		myVector.push_back(aObject);
	}

	template<typename Type>
	inline void List<Type>::Insert(const size_t aIndex, const Type &aObject)
	{
		myVector.insert(myVector.begin() + aIndex, aObject);
	}

	template<typename Type>
	inline void List<Type>::RemoveCyclic(const Type &aObject)
	{
		for (size_t i = 0; i < myVector.size(); ++i)
		{
			if (myVector[i] == aObject)
			{
				RemoveCyclicAtIndex(i);
				return;
			}
		}
	}

	template<typename Type>
	inline void List<Type>::RemoveCyclicAtIndex(const size_t aIndex)
	{
		myVector[aIndex] = myVector[myVector.size() - 1];
		myVector.erase(myVector.end() - 1);
	}

	template<typename Type>
	inline void List<Type>::RemoveAtIndex(const size_t aIndex)
	{
		myVector.erase(myVector.begin() + aIndex);
	}

	template<typename Type>
	inline void List<Type>::Clear()
	{
		myVector.clear();
	}

	template <typename Type>
	Type& List<Type>::back()
	{
		return myVector.back();
	}

	template<typename Type>
	inline constexpr size_t List<Type>::Count() const
	{
		return myVector.size();
	}

	template<typename Type>
	inline constexpr size_t List<Type>::Capacity() const
	{
		return myVector.capacity();
	}

	template<typename Type>
	inline constexpr Type *List<Type>::Data()
	{
		return myVector.data();
	}

	template<typename Type>
	inline constexpr const Type *List<Type>::Data() const
	{
		return myVector.data();
	}
}
