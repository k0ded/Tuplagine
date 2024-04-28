#include "tgpch.h"
#include "DepthProgram.h"

#include "Tupla/Renderer/Camera.h"
#include "Tupla/Renderer/Primitives/Mesh.h"
#include "Tupla/Renderer/RHI/RHI.h"

namespace Tupla
{
	void RunDepthPass(
		const std::vector<ModelGFXCommand>& modelsToRender, 
		const Camera& aCamera, 
		const Ref<Shader>& aVertexShader, 
		const Ref<ConstantBuffer>& aObjectBuffer,
		const Texture* outTexture
	)
	{
		ASSERT(outTexture != nullptr, "Cannot have a nulled output. Just doesnt make sense!");
		// Assuming that the frame and object buffer is already bound!!!

		aCamera.Update();
		RHI::SetRenderTarget(nullptr, outTexture);
		RHI::ClearDepthStencil(outTexture);
		RHI::SetPixelShader(nullptr);
		RHI::SetVertexShader(aVertexShader.get());

		for (auto& model : modelsToRender)
		{
			aObjectBuffer->SetData(model.PerObject);
			RHI::UpdateConstantBufferData(*aObjectBuffer);
			RHI::SetTopology(4);
			RHI::SetVertexAndIndexBuffers(model.MeshData->VXBuffer, model.MeshData->IXBuffer, sizeof(Vertex));
			RHI::DrawIndexed(model.MeshData->myIndexCount);
		}
	}
}
