#include "RuntimeLayer.h"

#include "CommonUtilities/Input/Input.h"
#include "CommonUtilities/Time/Time.h"
#include "Tupla/Core/Application.h"
#include "Tupla/AssetManager/Assets/Model.h"
#include "Tupla/Scene/SceneManager.h"

Tupla::RuntimeLayer::RuntimeLayer(): Layer("Game")
{
	std::vector<Vertex> vec;

	m_Mesh = Application::Get().GetAssetManager().GetAssetFromFile<Model>("model.fbx");

	//m_Mesh = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMesh();
	m_Material = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMaterial();

	Ref<Shader> shader = Application::Get().GetRenderer()->GetRenderingAssets()->CreateShader();
	shader->CompileShader(L"./gpu.hlsl", ShaderStage::Vertex);
	shader->CompileShader(L"./gpu.hlsl", ShaderStage::Pixel);

	m_Camera.SetFOV(90);
	m_Camera.SetFarPlane(1000.f);
	m_Camera.SetNearPlane(0.1f);
	// TEMP
	m_Texture = Application::Get().GetRenderer()->GetRenderingAssets()->CreateTexture();
	m_Material->SetShader(shader);
	m_Material->AttachImage(m_Texture);

	m_ConstantBuffer = m_Material->GetBuffer(static_cast<size_t>(VertexBufferType::PerObject), ShaderStageSlot::Vertex);

	m_Transform = CU::Matrix4x4<float>();
	m_Transform.SetPosition({ 0.f, 0.f, 4.f });
}

void Tupla::RuntimeLayer::OnUpdate()
{
	SceneManager::GetActiveScene().Update();

	PerObject perObj{};
	perObj.Transform = m_Transform;

	const auto vpSize = Application::Get().GetRenderer()->GetViewportSize();

	CommonUtilities::Matrix4x4<float> cameraTransform = CU::Matrix4x4<float>::ComposeMatrix(m_CameraPosition, m_CameraRotation, { 1.f, 1.f, 1.f });

	if(Input::GetKey(CU::KeyCode::MOUSE_RBUTTON))
	{
		float x = 0;
		float y = 0;
		float z = 0;

		if (Input::GetKey(CU::KeyCode::A)) x -= 100.f;
		if (Input::GetKey(CU::KeyCode::D)) x += 100.f;
		if (Input::GetKey(CU::KeyCode::W)) z += 100.f;
		if (Input::GetKey(CU::KeyCode::S)) z -= 100.f;
		if (Input::GetKey(CU::KeyCode::E)) y += 100.f;
		if (Input::GetKey(CU::KeyCode::Q)) y -= 100.f;

		CU::Vector2f mouseDelta = static_cast<CU::Vector2f>(Input::GetMouse().GetPositionDelta()) / static_cast<CU::Vector2f>(vpSize);
		CU::Vector3f rotationDifference = { mouseDelta.y, mouseDelta.x, 0 };

		auto pos = cameraTransform.GetPosition();
		auto forward = cameraTransform.GetForward();
		auto right = cameraTransform.GetRight();
		auto up = cameraTransform.GetUp();

		pos += forward * (z * Time::DeltaTime());
		pos += right * (x * Time::DeltaTime());
		pos += up * (y * Time::DeltaTime());

		m_CameraPosition = pos;
		m_CameraRotation += rotationDifference;
	}

	m_Camera.SetPosition(m_CameraPosition);
	m_Camera.SetRotation(m_CameraRotation * CommonUtilities::Math::RAD2DEG);
	m_Camera.SetDisplaySize(vpSize);
	m_Camera.PrepareFrame();

	perObj.LightVector = { 1.f, -1.f, 1.f };
	CU::Matrix4x4<float> transform = CU::Matrix4x4<float>();

	m_ConstantBuffer->WriteToBuffer(perObj);

	for (const auto& mesh : m_Mesh->m_Meshes)
	{
		Application::Get().GetRenderer()->RenderMesh(mesh, m_Material);
	}
}
