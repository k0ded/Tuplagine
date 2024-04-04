﻿#include "EditorLayer.h"
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
