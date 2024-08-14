#pragma once
#include "Asset.h"
#include "Tupla/Renderer/Primitives/Mesh.h"

namespace Tupla 
{
	// Default Serialization happens through AssetPostProcessors!
	class MeshAsset : public Asset
	{
	public:
        MeshAsset(const u64 aId): Asset(aId) {}
		
		void DeserializeAssetPacked(const std::byte* data, u64 dataSize) override;
		void SerializeAssetPacked(std::vector<std::byte>& outResult) override;
        void SetMeshes(const std::vector<Ref<Mesh>>&& aMeshes);

    private:
		std::vector<Ref<Mesh>> m_Mesh;
	};
}
