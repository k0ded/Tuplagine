#include "RuntimeLayer.h"

#include "CommonUtilities/Input/Input.h"
#include "CommonUtilities/Time/Time.h"
#include "Tupla/Core/Application.h"
#include "Tupla/Scene/SceneManager.h"

Tupla::RuntimeLayer::RuntimeLayer(): Layer("Game")
{

}

void Tupla::RuntimeLayer::OnUpdate()
{
	SceneManager::GetActiveScene().Update();
	Application::Get().GetRenderer()->ProcessCommands();
}
