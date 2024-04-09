#include "RuntimeLayer.h"

#include <iostream>

#include "Tupla/Core/Application.h"
#include "Tupla/AssetManager/Assets/Model.h"
#include "Tupla/Scene/SceneManager.h"

Tupla::RuntimeLayer::RuntimeLayer(): Layer("Game")
{
	std::vector<Vertex> vec;

	//m_Mesh = Application::Get().GetAssetManager().GetAssetFromFile<Model>("model.fbx")->m_Meshes[0];
	m_Mesh = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMesh();
	m_Material = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMaterial();

	Ref<Shader> shader = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader();
	shader->CompileShader(L"./gpu.hlsl", ShaderStage::Vertex);
	shader->CompileShader(L"./gpu.hlsl", ShaderStage::Pixel);

	// TEMP
	m_CameraBuffer = Application::Get().GetRenderer()->GetRenderingAssets()->CreateBuffer(sizeof(CameraData));

	m_Texture = Application::Get().GetRenderer()->GetRenderingAssets()->CreateTexture();
	m_Material->SetShader(shader);
	m_Material->AttachImage(m_Texture);
	m_Material->AttachBuffer(m_CameraBuffer, ShaderStageSlot::Vertex);   // SLOT 1

	m_ConstantBuffer = m_Material->GetBuffer(0, ShaderStageSlot::Vertex);

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
	PerObject perObj{};
	perObj.Transform = m_Transform;

	const auto vpSize = Application::Get().GetRenderer()->GetViewportSize();
	const auto ar = static_cast<float>(vpSize.x) / static_cast<float>(vpSize.y);

	perObj.LightVector = { 1.f, -1.f, 1.f };
	CU::Matrix4x4<float> transform = CU::Matrix4x4<float>();

	m_ConstantBuffer->WriteToBuffer(perObj);

	Application::Get().GetRenderer()->RenderMesh(m_Mesh, m_Material);
}
