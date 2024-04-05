#pragma once
#include "HotDefines.h"
#include "HotComponentCollection.h"
#include <unordered_map>

namespace Tupla
{
	class HotComponentManager
	{
	public:
		template<typename T>
		void RegisterComponent()
		{
			//assert(RUNTIME_STATIC_STORAGE<T>::mIndex == (size_t)-1 && "Registering component type more than once.");

			RUNTIME_STATIC_STORAGE<T>::mIndex = mNextComponentType;

			mComponentArrays.insert({ RUNTIME_STATIC_STORAGE<T>::mIndex, std::make_shared<HotComponentCollection<T>>() });
			++mNextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			assert(RUNTIME_STATIC_STORAGE<T>::mIndex != (size_t)-1 && "Component not registered before use.");

			return RUNTIME_STATIC_STORAGE<T>::mIndex;
		}

		template<typename T>
		void AddComponent(HOTEntity entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		T& GetComponent(HOTEntity entity)
		{
			// Get a reference to a component from the array for an entity
			return GetComponentArray<T>()->GetData(entity);
		}

		template<typename T>
		std::array<T, MAX_ENTITIES>& GetAllComponents()
		{
			// Get a reference to a component from the array for an entity
			return GetComponentArray<T>()->GetCollection();
		}

		template<typename T>
		std::array<T, MAX_ENTITIES>& GetComponentArray(HOTEntity entity)
		{
			return GetComponentArray<T>()->GetCollection();
		}

	private:
		std::unordered_map<ComponentType, std::shared_ptr<IHotComponentCollection>> mComponentArrays{};

		ComponentType mNextComponentType{};

		template<typename T>
		std::shared_ptr<HotComponentCollection<T>> GetComponentArray()
		{
			assert(RUNTIME_STATIC_STORAGE<T>::mIndex != (size_t)-1 && "Component not registered before use.");

			return std::static_pointer_cast<HotComponentCollection<T>>(mComponentArrays[RUNTIME_STATIC_STORAGE<T>::mIndex]);
		}
	};
}
