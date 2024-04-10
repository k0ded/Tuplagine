#pragma once

namespace Tupla
{
    class Asset
    {
    public:
        Asset(u64 aId): m_Id(aId) {}
        virtual ~Asset() = default;
        
        virtual void DeserializeAsset(const std::string& aFilePath) = 0;
        virtual void SerializeAsset(std::vector<std::byte>& outResult) = 0;
        virtual void DeserializeAssetPacked(const std::byte* data, u64 dataSize) = 0;
        virtual void SerializeAssetPacked(std::vector<std::byte>& outResult) = 0;

        friend class AssetManager;
    protected:
        u64 m_Id;
    };    
}
