#pragma once
#include "HotDefines.h"
#include "CommonUtilities/FixedSizeList.hpp"
#include "Tupla/Jobs/JobDefines.h"

#define READ_COMPONENT(x) m_ReadComponents.Add(RUNTIME_STATIC_STORAGE<x>::mIndex)
#define WRITE_COMPONENT(x) m_WriteComponents.Add(RUNTIME_STATIC_STORAGE<x>::mIndex)

namespace Tupla
{
	class HotCoordinator;

	class HotSystem
	{
	public:
		virtual ~HotSystem() = default;
		virtual void Update(HotCoordinator* coordinator) = 0;

		CU::FixedSizeList<ComponentType, MAX_THREADED_ACCESSES> m_WriteComponents;
		CU::FixedSizeList<ComponentType, MAX_THREADED_ACCESSES> m_ReadComponents;
		std::set<HOTEntity> m_Entities;
	};
}

