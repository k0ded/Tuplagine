#pragma once
#include "Tupla/Renderer/Material.h"

namespace Tupla
{
	struct MaterialComponent
	{
		std::array<Ref<Texture>, 8> Textures;
		Constants ObjectConstants;
	};
}
