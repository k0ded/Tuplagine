#pragma once
#include "CommonUtilities/File.h"

namespace Tupla
{
    class Asset
    {
    public:
        Asset(const u64 aId): m_VirtualID(aId) {}
        virtual ~Asset() = default;
        
        virtual void DeserializeAsset(const std::string& aFilePath) { CU::ReadFileBinary(aFilePath.c_str(), m_Data); }
        virtual void SerializeAsset(std::vector<std::byte>& outResult) { outResult = m_Data; }
        virtual void DeserializeAssetPacked(const std::byte* data, u64 dataSize) = 0;
        virtual void SerializeAssetPacked(std::vector<std::byte>& outResult) = 0;

        const std::vector<std::byte>& GetData() { return m_Data; }
        u64 GetID() const { return m_VirtualID; }
        //const std::string& GetVirtualPath() { return m_VirtualPath; }

        friend class AssetManager;
    protected:
        u64 m_VirtualID;
        //std::string m_VirtualPath; Possible continuation
        std::vector<std::byte> m_Data;
    };    
}
