#include "RuntimeLayer.h"

#include "CommonUtilities/Input/Input.h"
#include "CommonUtilities/Time/Time.h"
#include "Tupla/Core/Application.h"
#include "Tupla/Scene/SceneManager.h"

Tupla::RuntimeLayer::RuntimeLayer(): Layer("Game")
{
	std::vector<std::byte> bytes {};
	CU::ReadFileBinary(Application::Get().GetAssetManager().GetAssetPath("Internal/Shaders/").c_str(), bytes);
}

void Tupla::RuntimeLayer::OnUpdate()
{
	SceneManager::GetActiveScene().Update();
}
