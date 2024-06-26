﻿#include "tgpch.h"
#include "AssetManager.h"

#include <ranges>

#include "Tupla/Core/Application.h"

constexpr const char* VMAP_PATH = "data.vmb"; // Virtual Map Binary (VMB)
constexpr const char* HEADER = "TUP";

// VMB FORMAT
// Header		- 3 bytes "TUP"
// Version		- 8 bytes
// FileCount	- 4 bytes
// AssetCount	- 4 bytes
// Files		- xxx bytes
// Assets		- xxx bytes

// FILE FORMAT
// Length - 4 bytes
// PhysicalFile - Length bytes

// ASSET FORMAT
// AssetID		- 8 bytes
// PhysicalFile - 8 bytes (index into files)
// Offset		- 8 bytes
// Size			- 8 bytes
// Packed		- 1 byte

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

PACK(
struct SAVEDASSET
{
	u64 assetID = 0;
	u64 physicalIndex = 0;
	u64 offset = 0;
	u64 size = 0;
	bool packed = false;
};
)

constexpr int FILE_MIN_SIZE = 5;
constexpr int ASSET_SIZE = sizeof(SAVEDASSET);

Tupla::AssetManager::AssetManager(std::string CacheLocation): m_CacheLocation(std::move(CacheLocation)),
                                                              m_Version(0)
{
	LoadVirtualMap();
	m_AbsoluteCacheLocation = Application::Get().GetSpecification().WorkingDirectory + "\\" + m_CacheLocation;
}

std::string Tupla::AssetManager::GetAssetPath(const std::string& aString) const
{
	return Application::Get().GetSpecification().WorkingDirectory + "\\Assets\\" + aString;
}

std::string Tupla::AssetManager::GetCachePath(const std::string& aString) const
{
	return m_AbsoluteCacheLocation + "\\" + aString;
}

std::string Tupla::AssetManager::GetRootPath(const std::string& aString) const
{
	return Application::Get().GetSpecification().WorkingDirectory + "\\" + aString;
}

bool Tupla::AssetManager::LoadVirtualMap()
{
	const auto path = Application::Get().GetSpecification().WorkingDirectory + "\\" + VMAP_PATH;

	std::vector<std::byte> data;
	size_t dataSize = CU::ReadFileBinary(path.c_str(), data);

	// Make sure header that the header may be correct.
	if(dataSize < 3 + 8 + 4 + 4)
	{
		LOG_ERROR("Invalid VMAP structure!");
		return false;
	}

	// Check header
	if((char)data[0] != HEADER[0] || (char)data[1] != HEADER[1] || (char)data[2] != HEADER[2])
	{
		LOG_ERROR("Invalid VMAP header!");
		return false;
	}

	// Load version
	memcpy(&m_Version, &data[3], 8);

	int fileCount = 0;
	int assetCount = 0;
	memcpy(&fileCount, &data[11], 4);
	memcpy(&assetCount, &data[15], 4);

	std::vector<std::string> files(fileCount);

	int mPtr = 19;
	for (int i = 0; i < fileCount; ++i)
	{
		if(dataSize < mPtr + FILE_MIN_SIZE) // DONT FORGET NULL TERMINATOR!!!
		{
			LOG_ERROR("VMAP paths corrupt!");
			return false;
		}

		int strSize = 0;
		memcpy(&strSize, &data[mPtr], 4);
		files[i] = std::string((char*)&data[mPtr + 4]);

		if(files[i].size() != strSize)
		{
			LOG_ERROR("VMAP paths corrupt!");
			return false;
		}
		mPtr += 4 + strSize + 1; // last 1 to include the null terminator.
	}

	std::unordered_map<u64, AssetEntry> VMap{};

	for (int i = 0; i < assetCount; ++i)
	{
		if (dataSize < mPtr + ASSET_SIZE) // DONT FORGET NULL TERMINATOR!!!
		{
			LOG_ERROR("VMAP Assets corrupt!");
			return false;
		}

		SAVEDASSET obj;

		memcpy(&obj, &data[mPtr], ASSET_SIZE);
		VMap[obj.assetID] = {};
		VMap[obj.assetID].Size = obj.size;
		VMap[obj.assetID].Offset = obj.offset;
		VMap[obj.assetID].PhysicalFilePath = files[obj.physicalIndex];
		VMap[obj.assetID].IsPacked = obj.packed;

		mPtr += ASSET_SIZE;
	}

	m_VirtualToPhysicalMap = VMap;
	LOG_INFO("Loaded VMAP version: {}", m_Version);
	return true;
}

void Tupla::AssetManager::SaveVirtualMap()
{
	const auto path = Application::Get().GetSpecification().WorkingDirectory + "\\" + VMAP_PATH;

	size_t byteSize = 3 + 8 + 4 + 4;
	byteSize += m_VirtualToPhysicalMap.size() * ASSET_SIZE;

	std::vector<std::string> strings{};
	std::unordered_map<u64, u64> idToPhysicalIndex{};

	for (auto& [id, entry] : m_VirtualToPhysicalMap)
	{
		auto found = std::ranges::find(strings, entry.PhysicalFilePath);

		if(found == strings.end())
		{
			strings.push_back(entry.PhysicalFilePath);
			byteSize += 4; // Forgot the size specifier oopsie
			byteSize += entry.PhysicalFilePath.size() + 1; // We cannot forget the null terminator!
			idToPhysicalIndex[id] = strings.size() - 1;
		}
		else 
		{
			idToPhysicalIndex[id] = found - strings.begin();
		}
	}

	std::vector<std::byte> data(byteSize);

	data[0] = static_cast<std::byte>(HEADER[0]);
	data[1] = static_cast<std::byte>(HEADER[1]);
	data[2] = static_cast<std::byte>(HEADER[2]);
	memcpy(&data[3], &m_Version, sizeof(size_t));

	const int fileCount = (int)strings.size();
	const int assetCount = (int)m_VirtualToPhysicalMap.size();

	memcpy(&data[11], &fileCount, 4);
	memcpy(&data[15], &assetCount, 4);

	int mPtr = 19;
	for (const auto& p : strings)
	{
		int strSize = (int)p.size();
		memcpy(&data[mPtr], &strSize, 4);
		memcpy(&data[mPtr + 4], p.c_str(), strSize + 1);
		mPtr += 4 + strSize + 1;
	}

	for (const auto& [id, entry] : m_VirtualToPhysicalMap)
	{
		SAVEDASSET obj =
		{
			id,
			idToPhysicalIndex[id],
			entry.Offset,
			entry.Size,
			entry.IsPacked
		};

		memcpy(&data[mPtr], &obj, ASSET_SIZE);
		mPtr += ASSET_SIZE;
	}

	if(CU::WriteFileBinary(path.c_str(), data.data(), (u32)data.size()))
	{
		LOG_INFO("Successfully built VMAP binary! Located at: {}", path.c_str());
	}
	else
	{
		LOG_ERROR("Failed when writing VMAP binary!");
	}
}

void Tupla::AssetManager::BuildVirtualMap()
{
	const auto workingDirectory = Application::Get().GetSpecification().WorkingDirectory + "\\Assets\\";
	const auto cacheDirectory = Application::Get().GetSpecification().WorkingDirectory + "\\" + m_CacheLocation + "\\";

	if(workingDirectory.empty())
	{
		return;
	}

	CU::CreateDirectories(workingDirectory.c_str());

	std::vector<std::string> files {};
	std::vector<std::string> cachedFiles {};
	CU::FindAll(workingDirectory.c_str(), files);

	CU::FindAll(cacheDirectory.c_str(), cachedFiles);

	for (const auto& cached_file : cachedFiles)
	{
		u64 hash = HASH_RUNTIME_STR(cached_file.c_str());
		bool exists = false;

		for (const auto& file : files)
		{
			u64 ahash = HASH_RUNTIME_STR(file.c_str());

			if(ahash == hash)
			{
				exists = true;
				break;
			}
		}

		if(!exists)
		{
			if(!CU::RemoveFile(cached_file.c_str()))
			{
				LOG_WARN("Failed to remove cached file: {}", cached_file);
			}
		}
	}

	for (const auto& file : files)
	{
		auto fileName = file.substr(workingDirectory.size());

		const u64 hash = HASH_RUNTIME_STR(fileName.c_str());

		if (!m_VirtualToPhysicalMap.contains(hash))
		{
			const auto cacheLocation = cacheDirectory + fileName;
			m_VirtualToPhysicalMap[hash].PhysicalFilePath = fileName;

			if (CU::FileExists(cacheLocation.c_str()))
			{
				m_VirtualToPhysicalMap[hash].IsPacked = true;
				LOG_INFO("Found cached file: {}", cacheLocation);
			}
			else
			{
				LOG_INFO("Found file: {}", fileName);
			}
		}

		if (TryInvalidate(file))
		{
			LOG_INFO("Changes found in {}, Reimporting!", file.c_str());
		}
	}

	m_Version = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	LOG_INFO("VMAP Version: {}", m_Version);
}
