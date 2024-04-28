#pragma once
#include "RenderCommand.h"
#include "Tupla/Renderer/RHI/ObjectBuffer.h"

namespace Tupla
{
	class Mesh;

	struct ModelGFXCommand : RenderCommand
	{
		Ref<Mesh> MeshData;
		// TODO: Add material support!

		ObjectBuffer PerObject {};
		GUID ConnectedEntity {};
	};
}
