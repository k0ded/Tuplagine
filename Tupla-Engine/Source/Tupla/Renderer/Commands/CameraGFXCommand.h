#pragma once
#include "RenderCommand.h"
#include "Tupla/Renderer/Camera.h"

namespace Tupla
{
	class Texture;

	struct CameraGFXCommand : RenderCommand
	{
		Camera CameraObject;
		Ref<Texture> ColorTarget;
		Ref<Texture> DepthTarget;
	};
}
