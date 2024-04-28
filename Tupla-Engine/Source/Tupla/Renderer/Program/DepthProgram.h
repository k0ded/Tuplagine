#pragma once
#include "Tupla/Renderer/Commands/ModelGFXCommand.h"

namespace Tupla
{
	class Camera;
	class Shader;
	class ConstantBuffer;
	class Texture;

	void RunDepthPass(
		const std::vector<ModelGFXCommand>& modelsToRender,
		const Camera& aCamera,
		const Ref<Shader>& aVertexShader,
		const Ref<ConstantBuffer>& aObjectBuffer,
		const Texture* outTexture
	);
}
