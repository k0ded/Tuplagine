#include "EditorLayer.h"
#include <imgui.h>
#include <Tupla.h>
#include "Themes.h"

#include "Inspector.h"
#include "Hierarchy.h"
#include "Assets.h"
#include "Console.h"
#include "PackageManager.h"
#include "Tupla/Scene/SceneManager.h"

void Tupla::EditorLayer::OnGUI()
{
#pragma region Themes and Layout

	// Tghemes and Size
	Tuplitor::Themes themes;
	themes.SetTheme(Tuplitor::Theme::Tupla);
	themes.SetSize(Tuplitor::Size::Rounded);
	themes.SetFont(Tuplitor::Font::Tratex);

	// Docking
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow();

#pragma region Viewport

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Scene");

	if(ImGui::IsWindowHovered())
	{
		// Do camera movement!

	}

	auto wSize = ImGui::GetContentRegionAvail();
	// Queues viewport image resize
	void* imguiImage = Application::Get().GetRenderer()->GetViewportImage({ static_cast<unsigned>(wSize.x), static_cast<unsigned>(wSize.y) });
	ImGui::Image(imguiImage, wSize);
	ImGui::End();
	ImGui::PopStyleVar();
#pragma endregion

	ImGui::Begin("Create");
	if(ImGui::Button("Create Entity"))
	{
		SceneManager::GetActiveScene().CreateEntity();
	}
	ImGui::End();


#pragma region Inspector

	{
		ImGui::Begin("Inspector");
		ImGui::End();
	}

#pragma endregion

#pragma region Modules

	{
		Inspector inspector;
	}
	{
		Hierarchy hierarchy;
	}

	m_Assets.OnGUI();

	{
		Console console;
	}
	{
		PackageManager packageManager;
	} 
	{
		ImGui::Begin("Project Settings");
		// Can configure project settings.
		ImGui::End();
		//ProjectSettings projectSettings;
	} 
	{
		ImGui::Begin("Preferences");
		// Can configure editor preferences.
		ImGui::End();
		//Preferences preferences;
	} 
	{
		ImGui::Begin("Build Settings");
		// Can build the game
		ImGui::End();
		//BuildSettings buildSettings;
	}

#pragma endregion

	ImGui::ShowDemoWindow();
}

void Tupla::EditorLayer::OnDetach()
{
	Application::Get().GetAssetManager().SaveVirtualMap();
}
