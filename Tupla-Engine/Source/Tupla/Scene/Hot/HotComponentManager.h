#pragma once
#include "HotDefines.h"
#include "HotComponentCollection.h"

namespace Tupla
{
	template<typename T>
	struct THROWAWAY_STATIC_STORAGE
	{
		static inline ComponentType mIndex = -1;
	};

	class HotComponentManager
	{
	public:
		template<typename T>
		void RegisterComponent()
		{
			ASSERT(THROWAWAY_STATIC_STORAGE<T>::mIndex == (size_t)-1, "Registering component type more than once.");

			THROWAWAY_STATIC_STORAGE<T>::mIndex = mNextComponentType;

			mComponentArrays.insert({ THROWAWAY_STATIC_STORAGE<T>::mIndex, std::make_shared<HotComponentCollection<T>>() });
			++mNextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			ASSERT(THROWAWAY_STATIC_STORAGE<T>::mIndex != (size_t)-1, "Component not registered before use.");

			return THROWAWAY_STATIC_STORAGE<T>::mIndex;
		}

		template<typename T>
		void AddComponent(HOTEntity entity, T component)
		{
			// Add a component to the array for an entity
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		void RemoveComponent(HOTEntity entity)
		{
			// Remove a component from the array for an entity
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T& GetComponent(HOTEntity entity)
		{
			// Get a reference to a component from the array for an entity
			return GetComponentArray<T>()->GetData(entity);
		}

		void EntityDestroyed(HOTEntity entity) const
		{
			// Notify each component array that an entity has been destroyed
			// If it has a component for that entity, it will remove it
			for (auto const& pair : mComponentArrays)
			{
				auto const& component = pair.second;

				component->EntityDestroyed(entity);
			}
		}

	private:
		std::unordered_map<ComponentType, std::shared_ptr<IHotComponentCollection>> mComponentArrays{};

		ComponentType mNextComponentType{};

		template<typename T>
		std::shared_ptr<HotComponentCollection<T>> GetComponentArray()
		{
			ASSERT(THROWAWAY_STATIC_STORAGE<T>::mIndex != (size_t)-1, "Component not registered before use.");

			return std::static_pointer_cast<HotComponentCollection<T>>(mComponentArrays[THROWAWAY_STATIC_STORAGE<T>::mIndex]);
		}
	};
}
