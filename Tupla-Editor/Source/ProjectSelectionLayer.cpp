#include "ProjectSelectionLayer.h"

#include "imgui.h"
#include "Tupla/Core/Application.h"

char buffer[1024];

void Tupla::ProjectSelectionLayer::OnGUI()
{
	ImGui::Begin("Project Selection");
	ImGui::Text("Selection things here?");
	if(ImGui::Button("Build Virtual Map"))
	{
		Application::Get().GetAssetManager().BuildVirtualMap();
	}

	ImGui::InputText("Dir: ", buffer, 1024);

	if(ImGui::Button("Set Working Directory"))
	{
		Application::Get().GetSpecification().WorkingDirectory = buffer;
	}

	if (ImGui::Button("Save Map"))
	{
		Application::Get().GetAssetManager().SaveVirtualMap();
	}

	if (ImGui::Button("Load Map"))
	{
		Application::Get().GetAssetManager().LoadVirtualMap();
	}

	if (ImGui::Button("Restart editor"))
	{
		Application::Get().Shutdown(true);
	}
	ImGui::End();
}
