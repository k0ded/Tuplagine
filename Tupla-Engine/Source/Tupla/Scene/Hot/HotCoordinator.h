#pragma once
#include "HotComponentManager.h"
#include "HotEntityManager.h"
#include "HotSystemManager.h"
#include "CommonUtilities/FixedSizeList.hpp"

namespace Tupla
{
	class HotCoordinator
	{
	public:
		void Init()
		{
			// Create pointers to each manager
			mComponentManager = std::make_unique<HotComponentManager>();
			mEntityManager = std::make_unique<HotEntityManager>();
			mSystemManager = std::make_unique<HotSystemManager>();
		}


		// Entity methods
		HOTEntity CreateEntity()
		{
			return mEntityManager->CreateEntity();
		}

		void DestroyEntity(HOTEntity entity)
		{
			mEntityManager->DestroyEntity(entity);
			mSystemManager->EntityDestroyed(entity);
		}


		// Component methods
		template<typename T>
		void RegisterComponent()
		{
			mComponentManager->RegisterComponent<T>();
		}

		template<typename T>
		void AddComponent(HOTEntity entity, T component)
		{
			mComponentManager->AddComponent<T>(entity, component);

			auto signature = mEntityManager->GetSignature(entity);
			signature.set(mComponentManager->GetComponentType<T>(), true);
			mEntityManager->SetSignature(entity, signature);

			mSystemManager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		void RemoveComponent(HOTEntity entity)
		{
			auto signature = mEntityManager->GetSignature(entity);
			signature.set(mComponentManager->GetComponentType<T>(), false);
			mEntityManager->SetSignature(entity, signature);

			mSystemManager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& GetComponent(HOTEntity entity)
		{
			return mComponentManager->GetComponent<T>(entity);
		}

		template<typename T>
		std::array<T, MAX_ENTITIES>& GetAllComponents()
		{
			return mComponentManager->GetAllComponents<T>();
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			return mComponentManager->GetComponentType<T>();
		}


		// System methods
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			return mSystemManager->RegisterSystem<T>();
		}

		template<typename T>
		void SetSystemSignature(HOTSignature signature)
		{
			mSystemManager->SetSignature<T>(signature);
		}

		template<typename T>
		const CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& GetSystemReadAccesses() const
		{
			return mSystemManager->GetSystemReadAccesses<T>();
		}

		template<typename T>
		const CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& GetSystemWriteAccesses() const
		{
			return mSystemManager->GetSystemWriteAccesses<T>();
		}

	private:
		std::unique_ptr<HotComponentManager> mComponentManager;
		std::unique_ptr<HotEntityManager> mEntityManager;
		std::unique_ptr<HotSystemManager> mSystemManager;
	};
}
