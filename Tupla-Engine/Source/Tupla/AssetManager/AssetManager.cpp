#include "tgpch.h"
#include "AssetManager.h"

void Tupla::AssetManager::InsertFilePath(const std::string& phyiscalFile, const std::string& virtualFile)
{
	m_VirtualToPhysicalMap[HASH_RUNTIME_STR(virtualFile)] = { 0, ~((unsigned int)0), phyiscalFile };
}

void Tupla::AssetManager::ReadBinaryFromFile(const std::string& virtualFile, std::vector<char>& buffer)
{
	auto mapping = m_VirtualToPhysicalMap[HASH_RUNTIME_STR(virtualFile)];
	CommonUtilities::ReadFileBinary(mapping.PhysicalFilePath.c_str(), buffer, (u32)mapping.Offset, (u32)mapping.Size);
}
