#pragma once
#include <vector>

#include "Tupla/AssetManager/Assets/Asset.h"

namespace Tupla 
{
	class IAssetPostProcessor
	{
	public:
		virtual ~IAssetPostProcessor() = default;
		virtual void PostProcess(const std::vector<std::byte>& aData, Asset* aAsset) = 0;
	};
}
