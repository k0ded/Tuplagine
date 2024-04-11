#pragma once
#include <string>
#include <vector>

namespace Tupla
{
	enum class AssetType
	{
		Raw,
		Folder,
		Model,
		Texture
	};

	struct BrowserAsset
	{
		AssetType Type = AssetType::Raw;
		std::string Name;
		std::string Path;

		BrowserAsset* parentAsset = nullptr;
		std::vector<BrowserAsset> myChildAssets{};
	};

	class Assets
	{
	public:
		Assets();
		~Assets();

		void OnGUI();

	private:
		void RenderAssets(BrowserAsset* aAsset);
		void BuildAssets(BrowserAsset& aAsset);

		float m_Padding = 16.f;
		float m_ThumbnailSize = 256;
		float m_PanelWidth;

		BrowserAsset myRootAsset;
		BrowserAsset* myCurrentAsset = &myRootAsset;
	};
}

// Can reach and modify assets.
