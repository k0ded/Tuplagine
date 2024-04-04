#pragma once

namespace Tupla
{
    class Asset
    {
    public:
        Asset(int aId): m_Id(aId) {}
        virtual ~Asset() = default;
        
        virtual void DeserializeAsset(const std::string& aFilePath) = 0;
        virtual u64 SerializeAsset(std::byte** outResult) = 0;
        virtual void DeserializeAssetPacked(const std::byte* data, u64 dataSize) = 0;
        virtual u64 SerializeAssetPacked(std::byte** outResult) = 0;

    protected:
        int m_Id;
    };    
}
