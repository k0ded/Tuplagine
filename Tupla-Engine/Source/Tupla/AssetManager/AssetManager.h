﻿#pragma once
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

    template <typename T>
    inline void TransformAssetPath(std::string& ) {  }
    
    class AssetManager
    {
    public:
	    explicit AssetManager(std::string CacheLocation = "Cache");

        template <typename T> requires(std::is_base_of_v<Asset, T>)
    	void SaveAsset(const T& Asset, const std::string& aFilePath, bool packed = false, u32 aOffset = 0)
        {
            Asset.m_Id = HASH_RUNTIME_STR(aFilePath.c_str());
            m_VirtualToPhysicalMap[Asset.m_Id].PhysicalFilePath = aFilePath;
            SaveAsset(Asset, packed, aOffset);
        }

        template <typename T> requires(std::is_base_of_v<Asset, T>)
        void SaveAsset(const T& Asset, bool packed = false, u32 aOffset = 0)
        {
            if(m_VirtualToPhysicalMap.contains(Asset.m_Id))
            {
                std::vector<std::byte> data;
                if (packed) Asset.SerializeAssetPacked(data);
                else Asset.SerializeAsset(data);

                if(!CU::WriteFileBinary(GetAssetPath(m_VirtualToPhysicalMap[Asset.m_Id].PhysicalFilePath), data.data(), data.size(), aOffset))
                {
                    LOG_ERROR("Failed to save asset!");
                    return;
                }

                m_VirtualToPhysicalMap[Asset.m_Id].Size = data.size();
                m_VirtualToPhysicalMap[Asset.m_Id].Offset = aOffset;
            }
        }

        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetAssetFromID(int aId) const
        {
            return GetOrLoadAsset<T>(aId);
        }
        
        template <typename T, typename ...Args> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetAssetFromFile(const std::string& aFile, const Args&... args)
        {
            auto hash = HASH_RUNTIME_STR(aFile.c_str());

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

            return GetOrLoadAsset<T, Args...>(hash, args...);
        }

        bool TryInvalidate(const std::string& aFile)
        {
	        const auto hash = HASH_RUNTIME_STR(aFile.c_str());
            if (!m_VirtualToPhysicalMap.contains(hash)) return false;

            // For editor invalidation
            if (m_VirtualToPhysicalMap[hash].IsPacked && !m_ReadPackedAssets)
            {
                const auto rawTS = CU::GetFileTimeStamp(GetAssetPath(aFile).c_str());
                const auto packedTS = CU::GetFileTimeStamp(GetCachePath(aFile).c_str());

                if (packedTS < rawTS)
                {
                    // Invalidate!!!
                    m_VirtualToPhysicalMap[hash].IsPacked = false;
                    m_VirtualToPhysicalMap[hash].PhysicalFilePath = aFile;
                    m_VirtualToPhysicalMap[hash].Offset = 0;
                    m_VirtualToPhysicalMap[hash].Size = 0;
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
        template <typename T, typename ...Args> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetOrLoadAsset(u64 aId, const Args&... args)
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

            auto asset = std::make_shared<T>(aId, args...);

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
            		m_VirtualToPhysicalMap[aId].Size = data.size();
            	}
            }
        	else if(m_VirtualToPhysicalMap[aId].IsPacked && !m_ReadPackedAssets)
            {
                const auto& [Offset, Size, PhysicalFilePath, isPacked] = m_VirtualToPhysicalMap[aId];

                std::vector<std::byte> data;
                auto size = CU::ReadFileBinary((m_CacheLocation + "\\" + PhysicalFilePath).c_str(), data, (u32)Size, (u32)Offset);

                if (size == 0)
                {
                    if(TryInvalidate(PhysicalFilePath))
                    {
                        GetOrLoadAsset<T>(aId);
                    }
                    return asset;
                }
            	m_TemporaryAssetCache[aId] = asset;
            	asset->DeserializeAssetPacked(data.data(), size);
            }

        	return asset;
        }

        std::unordered_map<u64, std::weak_ptr<Asset>> m_TemporaryAssetCache{};
        std::unordered_map<u64, std::shared_ptr<Asset>> m_PersistentAssetCache{};
        std::unordered_map<u64, AssetEntry> m_VirtualToPhysicalMap{};
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
