#include "RuntimeLayer.h"

#include <iostream>

#include "Tupla/Core/Application.h"
#include "../xcube.h"
#include "Tupla/Scene/SceneManager.h"

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

			{ vertexdata[floatCursor + 8], vertexdata[floatCursor + 9], vertexdata[floatCursor + 10] }
			}
		);
		cursor++;
	}

	m_Mesh->CreateMesh(vec, indexdata);
	m_Material = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMaterial();

	Ref<Shader> vert = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader(L"./gpu.hlsl", ShaderStage::Vertex);
	Ref<Shader> ps = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader(L"./gpu.hlsl", ShaderStage::Pixel);

	m_Texture = Application::Get().GetRenderer()->GetRenderingAssets()->CreateTexture();
	m_Texture->SetImageData(texturedata, 2, 2);
	m_Material->SetShaderStage(vert);
	m_Material->SetShaderStage(ps);
	m_Material->AttachImage(m_Texture);


	m_Transform = CU::Matrix4x4<float>();
	m_Transform.SetPosition({ 0.f, 0.f, 4.f });
}

void Tupla::RuntimeLayer::OnUpdate()
{
	SceneManager::GetActiveScene().Update();

	m_Transform = CU::Matrix4x4<float>::CreateRotation({ 0.0005f, 0.0009f, 0.0001f }) * m_Transform;
	Constants consts{};
	consts.Transform = m_Transform;

	const auto vpSize = Application::Get().GetRenderer()->GetViewportSize();
	const auto ar = static_cast<float>(vpSize.x) / static_cast<float>(vpSize.y);

	float n = 1.f;
	float h = 1.f;
	float f = 9.f;

	consts.Projection = CU::Matrix4x4<float>(std::array<float, 16>{ 2 * n / ar, 0, 0, 0, 0, 2 * n / h, 0, 0, 0, 0, f / (f - n), 1, 0, 0, n * f / (n - f), 0 });

	//consts.Projection = CU::Matrix4x4<float>::CreatePerspectiveProjection(ar, 90.f * Nerd::DEG2RAD, 1000.f, 0.1f);
	consts.LightVector = { 1.f, -1.f, 1.f };
	CU::Matrix4x4<float> transform = CU::Matrix4x4<float>();
	m_Material->UpdateConstants(consts);

	Application::Get().GetRenderer()->RenderMesh(m_Mesh, m_Material);
}
