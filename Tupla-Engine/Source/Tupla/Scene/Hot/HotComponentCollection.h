#pragma once
#include "HotDefines.h"

namespace Tupla
{
	class IHotComponentCollection
	{
	public:
		virtual ~IHotComponentCollection() = default;
	};

	template <typename T>
	class HotComponentCollection : public IHotComponentCollection
	{
	public:
		void InsertData(HOTEntity entity, T component)
		{
			mComponentArray[entity] = component;
		}

		T& GetData(HOTEntity entity)
		{
			return mComponentArray[entity];
		}

		// Used when running simple systems on this collection
		std::array<T, MAX_ENTITIES>& GetCollection() { return mComponentArray; }

	private:
		std::array<T, MAX_ENTITIES> mComponentArray;
	};
}
