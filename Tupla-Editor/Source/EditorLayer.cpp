#include "EditorLayer.h"
#include <imgui.h>
#include <Tupla.h>
#include "Themes.h"

static float someFloat = 0.f;

void Tupla::EditorLayer::OnGUI()
{
	Tuplitor::Themes themes;
	themes.SetTheme(Tuplitor::Theme::Tupla);
	themes.SetSize(Tuplitor::Size::Rounded);
	themes.SetFont(Tuplitor::Font::Tratex);

	ImGui::ShowDemoWindow();
	ImGui::Begin("Something");
	ImGui::SliderFloat("slidername", &someFloat, 0, 1);
	ImGui::End();


#pragma region Inspector

	{
		ImGui::Begin("Inspector");
		ImGui::End();
	}

#pragma endregion

#pragma region Hierarchy

	{
		ImGui::Begin("Hierarchy");
		ImGui::End();
	}

#pragma endregion

#pragma region Assets

	{
		ImGui::Begin("Assets");
		ImGui::End();
	}

#pragma endregion

#pragma region Package Manager

	{
		ImGui::Begin("Package Manager");
		ImGui::End();
	}

#pragma endregion

#pragma region Project Settings

	{
		ImGui::Begin("Project Settings");
		ImGui::End();
	}

#pragma endregion

#pragma region Preferences

	{
		ImGui::Begin("Preferences");
		ImGui::End();
	}

#pragma endregion

#pragma region Build Settings

	{
		ImGui::Begin("Build Settings");
		ImGui::End();
	}

#pragma endregion
}
