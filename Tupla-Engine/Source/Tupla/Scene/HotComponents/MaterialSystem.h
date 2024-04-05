#pragma once
#include "Tupla/Scene/Hot/HotSystem.h"

namespace Tupla
{
	class MaterialSystem : public HotSystem
	{
	public:
		MaterialSystem();
		void Update(HotCoordinator* coordinator) override;
	};
}

