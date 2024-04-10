#include "tgpch.h"
#include "Model.h"

#include "CommonUtilities/File.h"
#include "openfbx/ofbx.h"
#include "Tupla/Core/Application.h"
#include "Tupla/Utils/StringUtils.h"

void Tupla::Model::DeserializeAsset(const std::string& aFilePath)
{
	std::vector<std::byte> data;
	std::string path = Application::Get().GetAssetManager().GetAssetPath(aFilePath);

	if(CU::ReadFileBinary(path.c_str(), data))
	{
		const char* needle = FindInsensitive(aFilePath, ".");

		if(!needle)
		{
			LOG_INFO("File ending not found: {}", aFilePath);
			return;
		}

		u64 fileEndingHash = HASH_RUNTIME_STR(needle);

		if(serializers.contains(fileEndingHash))
		{
			m_Meshes = serializers.at(fileEndingHash)->SerializeModel(path);
		}
		else
		{
			LOG_ERROR("File ending not supported: {}", aFilePath);
		}
	}
}

void Tupla::Model::SerializeAsset(std::vector<std::byte>& outResult) // TODO: Doesnt support saving of models in a raw format!
{
}


// Packed format
// Mesh count		- 4 bytes
// Mesh data		- xxx bytes

constexpr u64 WorkingMemory = 16192;

void Tupla::Model::DeserializeAssetPacked(const std::byte* data, u64 dataSize)
{
	if(dataSize < 4)
	{
		LOG_ERROR("Model data corrupt!");
		return;
	}

	u32 meshCount = *(u32*)data;

	u64 ptr = 4;

	for (u32 i = 0; i < meshCount; ++i)
	{
		Ref<Mesh> mesh = Application::Get().GetRenderer()->GetRenderingAssets()->CreateMesh();
		m_Meshes.push_back(mesh);
		mesh->CreateMesh(&data[ptr], dataSize - ptr);
	}
}

void Tupla::Model::SerializeAssetPacked(std::vector<std::byte>& outResult)
{
	outResult.resize(sizeof(u32));
	outResult.reserve(WorkingMemory);

	const u32 meshCount = (u32)m_Meshes.size();

	memcpy(&outResult[0], &meshCount, sizeof(u32));

	for (u64 i = 0; i < meshCount; ++i)
	{
		m_Meshes[i]->AppendMeshData(outResult);
	}
}
