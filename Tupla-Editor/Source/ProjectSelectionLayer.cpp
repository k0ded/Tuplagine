#include "ProjectSelectionLayer.h"

#include "imgui.h"
#include "Tupla/Core/Application.h"

char buffer[1024];

void Tupla::ProjectSelectionLayer::OnGUI()
{
	const auto size = Application::Get().GetRenderer()->GetWindow()->GetExtents();
	constexpr float headerSize = 100.f;

	ImVec2 vec = { static_cast<float>(size.x), static_cast<float>(size.y) };
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
	ImGui::Begin("Project Selection", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

	auto avail = ImGui::GetContentRegionAvail();

	// Header!
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.f,5.f });
	ImGui::BeginChild("#", { avail.x, headerSize - 5.f }, 0, ImGuiWindowFlags_NoMove);

	ImGui::Text("Text");

	ImGui::EndChild();
	ImGui::PopStyleVar();



	// Project selection area
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.f,5.f });
	ImGui::BeginChild("##", { avail.x, avail.y - headerSize }, ImGuiChildFlags_Border, ImGuiWindowFlags_NoMove);
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
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::End();
	ImGui::PopStyleVar();
}
