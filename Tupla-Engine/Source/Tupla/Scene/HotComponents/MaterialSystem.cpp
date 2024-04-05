#include "tgpch.h"
#include "MaterialSystem.h"

#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "Tupla/Scene/Hot/HotCoordinator.h"

Tupla::MaterialSystem::MaterialSystem()
{
	READ_COMPONENT(MaterialComponent);
	READ_COMPONENT(TransformComponent);

	WRITE_COMPONENT(MaterialComponent);
}

void Tupla::MaterialSystem::Update(HotCoordinator* coordinator)
{
	auto& material = coordinator->GetAllComponents<MaterialComponent>();
	const auto& transform = coordinator->GetAllComponents<TransformComponent>();

	for (const auto entity : m_Entities)
	{
		auto& mat = material[entity];
		const auto& trans = transform[entity];

		CU::Matrix4x4<float> tRot = CU::Matrix4x4<float>::CreateRotation(trans.WorldRotationRad);
		//CU::Matrix4x4<float> tScale = CU::Matrix4x4<float>::CreateRotation(trans.WorldScale);
		CU::Matrix4x4<float> tPos = CU::Matrix4x4<float>::CreateRotation(trans.WorldPosition);

		mat.ObjectConstants.Transform = tPos * tRot;
		mat.ObjectConstants.LightVector = { 1.f, -1.f, 1.f };
	}
}
