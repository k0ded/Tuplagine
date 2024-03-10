#pragma once

namespace Tupla
{
    class Asset
    {
    public:
        Asset(int aId): m_Id(aId) {}
        virtual ~Asset() = default;
        
        virtual void DeserializeAsset(const std::string& aFilePath) = 0;
        virtual std::string SerializeAsset() = 0;
        virtual void DeserializeAssetPacked(const std::string& data) = 0;
        virtual std::string SerializeAssetPacked() = 0;

    protected:
        int m_Id;
    };    
}
