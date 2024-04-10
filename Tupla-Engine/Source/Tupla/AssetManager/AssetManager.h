#pragma once
#include <unordered_map>

#include "Assets/Asset.h"
#include "CommonUtilities/File.h"
#include "Tupla/Utils/Hashing.h"
#include "Tupla/Core/Log.h"

namespace Tupla
{
    struct AssetEntry
    {
        u64 Offset = 0;
        u64 Size = 0;
        std::string PhysicalFilePath;
        bool IsPacked = false;
    };
    
    class AssetManager
    {
    public:
	    explicit AssetManager(std::string CacheLocation = "Cache");

        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetAssetFromID(int aId) const
        {
            return GetOrLoadAsset<T>(aId);
        }
        
        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetAssetFromFile(const std::string& aFile)
        {
            auto hash = HASH_RUNTIME_STR(aFile);

            if (!m_VirtualToPhysicalMap.contains(hash))
            {
            	const auto cacheLocation = m_CacheLocation + "\\" + aFile;
                m_VirtualToPhysicalMap[hash].PhysicalFilePath = aFile;

                if (CU::FileExists(cacheLocation.c_str()))
                {
                    m_VirtualToPhysicalMap[hash].IsPacked = true;
                }
            }

        	if (TryInvalidate(aFile))
        	{
        		LOG_INFO("Changes found in {}, Reimporting!", aFile.c_str());
        	}

            return GetOrLoadAsset<T>(hash);
        }

        bool TryInvalidate(const std::string& aFile)
        {
	        const auto hash = HASH_RUNTIME_STR(aFile);
            if (!m_VirtualToPhysicalMap.contains(hash)) return false;

            // For editor invalidation
            if (m_VirtualToPhysicalMap[hash].IsPacked && !m_ReadPackedAssets)
            {
                const auto rawTS = CU::GetFileTimeStamp(aFile.c_str());
                const auto packedTS = CU::GetFileTimeStamp(m_VirtualToPhysicalMap[hash].PhysicalFilePath.c_str());

                if (packedTS < rawTS)
                {
                    // Invalidate!!!
                    m_VirtualToPhysicalMap[hash].IsPacked = false;
                    m_VirtualToPhysicalMap[hash].PhysicalFilePath = aFile;
                    return true;
                }
            }

            return false;
        }

        std::string GetAssetPath(const std::string& aString) const;
        std::string GetCachePath(const std::string& aString) const;
    	std::string GetRootPath(const std::string& aString) const;

        bool LoadVirtualMap();
        void SaveVirtualMap();
        void BuildVirtualMap();

    private:
        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetOrLoadAsset(const int aId)
        {
            // These are the most likely to hit so try them first!
            if (m_TemporaryAssetCache.contains(aId))
            {
                // We might not have gone through a collection pass yet!
                if(!m_TemporaryAssetCache.at(aId).expired())
                {
                    return std::static_pointer_cast<T, Asset>(m_TemporaryAssetCache.at(aId).lock());
                }
            }

        	if(m_PersistentAssetCache.contains(aId))
            {
                return std::static_pointer_cast<T, Asset>(m_PersistentAssetCache.at(aId));
            }

            if(!m_VirtualToPhysicalMap.contains(aId))
            {
                throw std::exception("Accessing an asset that doesn't exist!");
            }

            auto asset = std::make_shared<T>(aId);

            if(!m_VirtualToPhysicalMap[aId].IsPacked && !m_ReadPackedAssets)
            {
                asset->DeserializeAsset(m_VirtualToPhysicalMap[aId].PhysicalFilePath);
                
            	// Build cache for editor!
            	std::vector<std::byte> data;
            	asset->SerializeAssetPacked(data);

                const auto cacheLocation = m_CacheLocation + "\\" + m_VirtualToPhysicalMap[aId].PhysicalFilePath;

            	if (CU::WriteFileBinary(cacheLocation.c_str(), data.data(), static_cast<u32>(data.size())))
            	{
            		m_VirtualToPhysicalMap[aId].IsPacked = true;
            	}
            }

            if(m_VirtualToPhysicalMap[aId].IsPacked && !m_ReadPackedAssets)
            {
                const auto& [Offset, Size, PhysicalFilePath, isPacked] = m_VirtualToPhysicalMap[aId];

                std::vector<std::byte> data;
                auto size = CU::ReadFileBinary((m_CacheLocation + "\\" + PhysicalFilePath).c_str(), data, Size, Offset);

            	m_TemporaryAssetCache[aId] = asset;
            	asset->DeserializeAssetPacked(data.data(), size);
            }

        	return asset;
        }

        std::unordered_map<u32, std::weak_ptr<Asset>> m_TemporaryAssetCache{};
        std::unordered_map<u32, std::shared_ptr<Asset>> m_PersistentAssetCache{};
        std::unordered_map<u32, AssetEntry> m_VirtualToPhysicalMap{};
        std::string m_CacheLocation;
        std::string m_AbsoluteCacheLocation;
        size_t m_Version;
        
#ifdef DIST
        constexpr static bool m_ReadPackedAssets = true;
#else
        constexpr static bool m_ReadPackedAssets = false;
#endif
    };
}
