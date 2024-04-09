#pragma once
#include "Asset.h"
#include "Tupla/AssetManager/RawFileSerializer/FBXSerializer.h"
#include "Tupla/AssetManager/RawFileSerializer/IModelSerializer.h"
#include "Tupla/Renderer/Material.h"
#include "Tupla/Renderer/Mesh.h"

namespace Tupla
{
	class Model : public Asset
	{
	public:
		Model(const u32 aId) : Asset(aId) {}

		void DeserializeAsset(const std::string& aFilePath) override;
		void SerializeAsset(std::vector<std::byte>& outResult) override;

		void DeserializeAssetPacked(const std::byte* data, u64 dataSize) override;
		void SerializeAssetPacked(std::vector<std::byte>& outResult) override;

		std::vector<Ref<Mesh>> m_Meshes {};
		std::vector<Ref<Material>> m_Materials {};
		std::vector<u32> m_MaterialIDs {};

	private:
		static inline std::unordered_map<int, IModelSerializer*> serializers =
		{
			{ HASH_STR(".fbx"), new FBXSerializer() }
		};
	};
}
