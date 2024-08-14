#include "tgpch.h"
#include "MeshAsset.h"

// Mesh Packing
// 8 bytes - Vertex Data Size
// x bytes - Vertex Data
// 8 bytes - Index Data Size
// x bytes - Index Data
void Tupla::MeshAsset::DeserializeAssetPacked(const std::byte* data, u64 dataSize)
{
}

void Tupla::MeshAsset::SerializeAssetPacked(std::vector<std::byte>& outResult)
{
}

void Tupla::MeshAsset::SetMeshes(const std::vector<Ref<Mesh>>&& aMeshes)
{
    m_Mesh = aMeshes;
}
