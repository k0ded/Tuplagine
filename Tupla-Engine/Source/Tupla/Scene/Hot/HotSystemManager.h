#pragma once
#include "HotDefines.h"
#include "HotSystem.h"
#include "CommonUtilities/FixedSizeList.hpp"

namespace Tupla
{
	class HotSystemManager
	{
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			uint8_t& index = RUNTIME_STATIC_STORAGE<T>::mIndex;

			assert(index == (uint8_t)-1 && "Registering system more than once.");

			index = m_NextSystemType++;

			// Create a pointer to the system and return it so it can be used externally
			auto system = std::make_shared<T>();
			m_Systems.insert({ index, system });
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			uint8_t& index = RUNTIME_STATIC_STORAGE<T>::mIndex;

			assert(index != (uint8_t) -1 && "System used before registered.");

			// Set the signature for this system
			m_Signatures.insert({ index, signature });
		}

		void EntityDestroyed(HOTEntity entity)
		{
			// Erase a destroyed entity from all system lists
			// mEntities is a set so no check needed
			for (auto const& pair : m_Systems)
			{
				auto const& system = pair.second;

				system->m_Entities.erase(entity);
			}
		}

		void EntitySignatureChanged(HOTEntity entity, Signature entitySignature)
		{
			// Notify each system that an entity's signature changed
			for (auto const& pair : m_Systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = m_Signatures[type];

				// Entity signature matches system signature - insert into set
				if ((entitySignature & systemSignature) == systemSignature)
				{
					system->m_Entities.insert(entity);
				}
				// Entity signature does not match system signature - erase from set
				else
				{
					system->m_Entities.erase(entity);
				}
			}
		}

		template<typename T>
		CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& GetSystemReadAccesses()
		{
			uint8_t& index = RUNTIME_STATIC_STORAGE<T>::mIndex;

			assert(index != (uint8_t)-1 && "System used before registered.");
			return m_Systems[index]->m_ReadComponents;
		}

		template<typename T>
		CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& GetSystemWriteAccesses()
		{
			uint8_t& index = RUNTIME_STATIC_STORAGE<T>::mIndex;

			assert(index != (uint8_t)-1 && "System used before registered.");
			return m_Systems[index]->m_WriteComponents;
		}

	private:
		// Map from system type string pointer to a signature
		std::unordered_map<SystemType, Signature> m_Signatures{};
		std::unordered_map<SystemType, std::shared_ptr<HotSystem>> m_Systems{};

		SystemType m_NextSystemType = 0;
	};
}
