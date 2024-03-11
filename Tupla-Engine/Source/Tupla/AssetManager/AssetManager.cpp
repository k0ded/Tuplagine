#include "tgpch.h"
#include "AssetManager.h"

void Tupla::AssetManager::ReadBinaryFromFile(const std::string& virtualFile, std::vector<char>& buffer)
{
	auto mapping = m_VirtualToPhysicalMap[HASH_RUNTIME_STR(virtualFile)];
	CommonUtilities::ReadFileBinary(mapping.PhysicalFilePath.c_str(), buffer, mapping.Offset, mapping.Size);
}
