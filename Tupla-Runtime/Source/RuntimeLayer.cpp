#include "RuntimeLayer.h"

#include "Tupla/Core/Application.h"
#include "../xcube.h"

Tupla::RuntimeLayer::RuntimeLayer(): Layer("Game")
{
	m_Mesh = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMesh();

	std::vector<Vertex> vec;

	int cursor = 0;
	while(cursor < sizeof(vertexdata) / sizeof(Vertex))
	{
		const int floatCursor = cursor * sizeof(Vertex) / sizeof(float);
		vec.push_back(Tupla::Vertex{
			{ vertexdata[floatCursor], vertexdata[floatCursor + 1], vertexdata[floatCursor + 2] },
			{ vertexdata[floatCursor + 3], vertexdata[floatCursor + 4], vertexdata[floatCursor + 5] },
			{ vertexdata[floatCursor + 6], vertexdata[floatCursor + 7] },
			{ vertexdata[floatCursor + 8], vertexdata[floatCursor + 9], vertexdata[floatCursor] }
			}
		);
		cursor++;
	}

	m_Mesh->CreateMesh(vec, indexdata);
	m_Material = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMaterial();

	Ref<Shader> vert = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader(L"./gpu.hlsl", ShaderStage::Vertex);
	Ref<Shader> ps = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader(L"./gpu.hlsl", ShaderStage::Pixel);
	Ref<Texture> tex = Application::Get().GetRenderer()->GetRenderingAssets()->CreateTexture();
	tex->SetImageData(texturedata, 2, 2);
	m_Material->SetShaderStage(vert);
	m_Material->SetShaderStage(ps);
	m_Material->AttachImage(tex);
}

void Tupla::RuntimeLayer::OnUpdate()
{
	Constants consts{};
	consts.Transform = CU::Matrix4x4<float>();
	consts.Transform.SetPosition({ 0.f, 0.f, 20.f });

	const auto viewportSize = Application::Get().GetRenderer()->GetViewportSize();
	const auto ar = static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y);
	consts.Projection = CU::Matrix4x4<float>::CreatePerspectiveProjection(ar, 45.f * Nerd::DEG2RAD, 1000.f, 0.1f);
	consts.LightVector = { 1.f, -1.f, 1.f };
	CU::Matrix4x4<float> transform = CU::Matrix4x4<float>();
	m_Material->UpdateConstants(consts);

	Application::Get().GetRenderer()->RenderMesh(m_Mesh, m_Material);
}
