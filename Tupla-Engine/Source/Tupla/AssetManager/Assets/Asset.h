#pragma once

namespace Tupla
{
    class Asset
    {
    public:
        Asset(u32 aId): m_Id(aId) {}
        virtual ~Asset() = default;
        
        virtual void DeserializeAsset(const std::string& aFilePath) = 0;
        virtual void SerializeAsset(std::vector<std::byte>& outResult) = 0;
        virtual void DeserializeAssetPacked(const std::byte* data, u64 dataSize) = 0;
        virtual void SerializeAssetPacked(std::vector<std::byte>& outResult) = 0;

    protected:
        u32 m_Id;
    };    
}
