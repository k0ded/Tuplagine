#include "tgpch.h"
#include "Camera.h"

#include "DX11/Primitives/DX11Material.h"

void Tupla::Camera::SetDisplaySize(CU::Vector2ui aResolution)
{
	m_Aspect = static_cast<float>(aResolution.x) / static_cast<float>(aResolution.y);
	m_PerspectiveDirty = true;
}

void Tupla::Camera::SetFOV(float aHorizontalFOV)
{
	m_FOV = aHorizontalFOV;
	m_PerspectiveDirty = true;
}

void Tupla::Camera::SetFarPlane(float aFar)
{
	m_Far = aFar;
	m_PerspectiveDirty = true;
}

void Tupla::Camera::SetNearPlane(float aNear)
{
	m_Near = aNear;
	m_PerspectiveDirty = true;
}

void Tupla::Camera::SetPosition(CU::Vector3f aPosition)
{
	m_Position = aPosition;
}

void Tupla::Camera::SetRotation(CU::Vector3f aRotation)
{
	m_Rotation = aRotation;
}

void Tupla::Camera::PrepareFrame()
{
	Ref<Buffer> frameBuffer = DX11Material::GetGlobalBuffer(static_cast<u64>(VertexBufferType::PerFrame), ShaderStageSlot::Vertex);

	if(m_PerspectiveDirty)
	{
		m_Perspective = CU::Matrix4x4<float>::CreatePerspectiveProjection(m_Aspect, m_FOV, m_Far, m_Near);
		m_PerspectiveDirty = false;
	}

	auto worldToView = CommonUtilities::Matrix4x4<float>::ComposeMatrix(m_Position, m_Rotation * CommonUtilities::Math::DEG2RAD, { 1.f, 1.f, 1.f }).GetFastInverse();

	PerFrame perFrame =
	{
		worldToView * m_Perspective,
		worldToView,
	};

	DX11Material::GetGlobalBuffer(static_cast<u64>(VertexBufferType::PerFrame), ShaderStageSlot::Vertex)->WriteToBuffer(perFrame);
}
