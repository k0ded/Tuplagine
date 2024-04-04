#pragma once
#include "HotDefines.h"

namespace Tupla
{
	class IHotComponentCollection
	{
	public:
		virtual ~IHotComponentCollection() = default;
		virtual void EntityDestroyed(HOTEntity entity) = 0;
	};

	template <typename T>
	class HotComponentCollection : public IHotComponentCollection
	{
	public:
		void InsertData(HOTEntity entity, T component)
		{
			assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

			// Put new entry at end and update the maps
			size_t newIndex = mSize;
			mEntityToIndexMap[entity] = newIndex;
			mIndexToEntityMap[newIndex] = entity;
			mComponentArray[newIndex] = component;
			++mSize;
		}

		void RemoveData(HOTEntity entity)
		{
			assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
			size_t indexOfLastElement = mSize - 1;
			mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

			// Update map to point to moved spot
			HOTEntity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
			mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			mEntityToIndexMap.erase(entity);
			mIndexToEntityMap.erase(indexOfLastElement);

			--mSize;
		}

		T& GetData(HOTEntity entity)
		{
			assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

			// Return a reference to the entity's component
			return mComponentArray[mEntityToIndexMap[entity]];
		}

		void EntityDestroyed(HOTEntity entity) override
		{
			if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
			{
				// Remove the entity's component if it existed
				RemoveData(entity);
			}
		}

		// Used when running systems on this collection
		std::array<T, MAX_ENTITIES>& GetCollection() { return mComponentArray; }
		size_t GetSize() const { return mSize; }

	private:
		std::array<T, MAX_ENTITIES> mComponentArray;

		// I changed my mind...
		// This way we get to iterate the whole map faster as they are closely packed -> Less cache misses due to invalidity
		std::unordered_map<HOTEntity, size_t> mEntityToIndexMap;
		std::unordered_map<size_t, HOTEntity> mIndexToEntityMap;

		size_t mSize = 0;
	};
}
