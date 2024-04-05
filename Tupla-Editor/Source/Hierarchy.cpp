#include "Hierarchy.h"

#include "Tupla/Scene/SceneManager.h"

Hierarchy::Hierarchy()
{
	ImGui::Begin("Hierarchy");

	ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	if(ImGui::BeginTable("Layout", 1, flags))
	{
		for (auto& entity: Tupla::SceneManager::GetActiveScene().GetEntities())
		{
			ImGui::TableNextColumn();
			ImGui::Button(entity.GetName(), {ImGui::GetColumnWidth(), 0});
		}

		ImGui::EndTable();
	}
}

Hierarchy::~Hierarchy()
{
	ImGui::End();
}
