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

		int fileEndingHash = HASH_RUNTIME_STR(needle);

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

void Tupla::Model::SerializeAsset(std::vector<std::byte>& outResult)
{

}

void Tupla::Model::DeserializeAssetPacked(const std::byte* data, u64 dataSize)
{
}

void Tupla::Model::SerializeAssetPacked(std::vector<std::byte>& outResult)
{
}

bool HasTangents(const ofbx::Mesh& aMesh)
{
	const ofbx::GeometryData& geometry = aMesh.getGeometryData();
	if (geometry.getTangents().values) return true;
	if (geometry.getUVs().values) return true;
	return false;
}
