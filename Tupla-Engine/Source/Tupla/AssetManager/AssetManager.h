#pragma once
#include <unordered_map>
#include <Tupla/Core/Base.h>

#include "Assets/Asset.h"
#include "CommonUtilities/File.h"
#include "Tupla/Utils/Hashing.h"

namespace Tupla
{
    struct AssetEntry
    {
        u64 Offset;
        u64 Size;
        std::string PhysicalFilePath;
    };
    
    class AssetManager
    {
    public:
        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetAssetFromID(int aId) const
        {
            return GetOrLoadAsset<T>(aId);
        }
        
        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetAssetFromFile(const std::string& aFile) const
        {
            return GetOrLoadAsset<T>(HASH_STR(aFile.c_str()));
        }

    private:
        template <typename T> requires(std::is_base_of_v<Asset, T>)
        std::shared_ptr<T> GetOrLoadAsset(const int aId)
        {
            // These are the most likely to hit so try them first!
            if(m_PersistentAssetCache.contains(aId))
            {
                return m_PersistentAssetCache[aId];
            }

            if(m_TemporaryAssetCache.contains(aId))
            {
                return m_PersistentAssetCache[aId];
            }

            if(!m_VirtualToPhysicalMap.contains(aId))
            {
                throw std::exception("Accessing an asset that doesn't exist!");
            }

            auto asset = std::make_shared<T>();

            if(!m_ReadPackedAssets)
            {
                asset->DeserializeAsset(m_VirtualToPhysicalMap[aId].PhysicalFilePath);
            }
            else
            {
                std::string str;
                const auto& [Offset, Size, PhysicalFilePath] = m_VirtualToPhysicalMap[aId];
                if(CU::ReadFile(PhysicalFilePath.c_str(), str, Offset, Size))
                {
                    m_TemporaryAssetCache[aId] = asset;
                
                    if(m_ReadPackedAssets)
                    {
                        asset->DeserializeAssetPacked(str);
                    }
                
                    return asset;
                }
            }
            
            throw std::exception("Failed to load asset!");
        }

        std::unordered_map<i32, std::weak_ptr<Asset>> m_TemporaryAssetCache;
        std::unordered_map<i32, std::shared_ptr<Asset>> m_PersistentAssetCache;
        std::unordered_map<i32, AssetEntry> m_VirtualToPhysicalMap;
        
#ifdef DIST
        bool m_ReadPackedAssets = true;
#else
        bool m_ReadPackedAssets = false;
#endif
    };
}
