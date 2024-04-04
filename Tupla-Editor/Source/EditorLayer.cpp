#include "EditorLayer.h"
#include <imgui.h>
#include <Tupla.h>
#include "Themes.h"

#include "Inspector.h"
#include "Hierarchy.h"
#include "Assets.h"
#include "Console.h"
#include "PackageManager.h"

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
	auto wSize = ImGui::GetContentRegionAvail();
	// Queues viewport image resize
	// Math to make the viewport stay 16:9			// Should be for game and not scene, should be able to configure window padding, should affect neighbouring docks
	bool lockAspectRatio = true;
	if (lockAspectRatio)
	{
		auto xScale = wSize.x / 16.f;
		auto yScale = wSize.y / 9.f;

		if (xScale > yScale) {

		wSize.x = (16.f / 9.f * wSize.y);
		}
		else {
		wSize.y = (9.f / 16.f * wSize.x);

		}
	}
	void* imguiImage = Application::Get().GetRenderer()->GetViewportImage({ static_cast<unsigned>(wSize.x), static_cast<unsigned>(wSize.y) });
	ImGui::Image(imguiImage, wSize);
	ImGui::End();
	ImGui::PopStyleVar();
#pragma endregion


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
	{
		Assets assets;
	}
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
