#include "Assets.h"
#include <imgui.h>

#include "CommonUtilities/File.h"
#include "Tupla/Core/Application.h"

namespace Tupla
{
	Assets::Assets()
	{
		myRootAsset.Path = Tupla::Application::Get().GetAssetManager().GetAssetPath("");
		myRootAsset.Type = AssetType::Folder;

		BuildAssets(myRootAsset);
	}

	Assets::~Assets()
	{
	}

	void Assets::OnGUI()
	{
		ImGui::Begin("Content Browser");
		if(myCurrentAsset != &myRootAsset)
		{
			if(ImGui::Button("<-"))
			{
				myCurrentAsset = myCurrentAsset->parentAsset;
			}
		}

		m_PanelWidth = ImGui::GetContentRegionAvail().x;
		float cellSize = m_ThumbnailSize + m_Padding;
		int columns = (int)(m_PanelWidth / cellSize);
		ImGui::Columns(columns >= 1 ? columns : 1, 0, false);
		RenderAssets(myCurrentAsset);
		ImGui::Columns(1);
		ImGui::SliderFloat("Thumbnail Size", &m_ThumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &m_Padding, 0, 32);

		ImGui::End();
	}

	void Assets::RenderAssets(BrowserAsset* aAsset)
	{
		if (aAsset == nullptr) return;

		for (auto& tab : aAsset->myChildAssets)
		{

			ImGui::Button(tab.Name.c_str(), { m_ThumbnailSize, m_ThumbnailSize });

			if(tab.Type == AssetType::Folder)
			{
				if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					LOG_INFO("Double click!");
					myCurrentAsset = &tab;
				}
			}
			else
			{
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					LOG_INFO("Entering file!");
				}
			}

			ImGui::Text(tab.Name.c_str());

			ImGui::NextColumn();
		}
	}

	void Assets::BuildAssets(BrowserAsset& aAsset)
	{
		std::vector<std::string> folders{};
		std::vector<std::string> files{};

		CU::FindAll(aAsset.Path.c_str(), folders, CommonUtilities::PathType::Directory, false);
		CU::FindAll(aAsset.Path.c_str(), files, CommonUtilities::PathType::File, false);

		BrowserAsset asset;
		asset.parentAsset = &aAsset;
		for (const auto& folder : folders)
		{
			asset.Name = folder.substr(aAsset.Path.size());
			asset.Path = folder + "\\";
			asset.Type = AssetType::Folder;
			BuildAssets(asset);
			aAsset.myChildAssets.push_back(asset);
		}

		for (const auto& file : files)
		{
			asset.Name = file.substr(aAsset.Path.size());
			asset.Path = file;
			asset.Type = AssetType::Model;
			aAsset.myChildAssets.push_back(asset);
		}
	}
}
