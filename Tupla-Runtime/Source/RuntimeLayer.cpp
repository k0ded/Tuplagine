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

	Ref<Shader> shader = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader();
	shader->CompileShader(L"./gpu.hlsl", ShaderStage::Vertex);
	shader->CompileShader(L"./gpu.hlsl", ShaderStage::Pixel);

	m_ConstantBuffer = Application::Get().GetRenderer()->GetRenderingAssets()->CreateBuffer(sizeof(Constants));
	m_CameraBuffer = Application::Get().GetRenderer()->GetRenderingAssets()->CreateBuffer(sizeof(CameraData));

	m_Texture = Application::Get().GetRenderer()->GetRenderingAssets()->CreateTexture();
	m_Texture->SetImageData(texturedata, 2, 2);
	m_Material->SetShader(shader);
	m_Material->AttachImage(m_Texture);
	m_Material->AttachBuffer(m_ConstantBuffer, ShaderStageSlot::Vertex); // SLOT 0
	m_Material->AttachBuffer(m_CameraBuffer, ShaderStageSlot::Vertex);   // SLOT 1


	CameraData data{};
	data.worldToClip = CU::Matrix4x4<float>::CreatePerspectiveProjection((16.f / 9.f), 90.f * Nerd::DEG2RAD, 1000.f, 0.1f);
	m_CameraBuffer->WriteToBuffer(data);

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

	consts.LightVector = { 1.f, -1.f, 1.f };
	CU::Matrix4x4<float> transform = CU::Matrix4x4<float>();

	m_ConstantBuffer->WriteToBuffer(consts);

	Application::Get().GetRenderer()->RenderMesh(m_Mesh, m_Material);
}
