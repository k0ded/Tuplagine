#pragma once
#include "Asset.h"
#include "Tupla/Renderer/Texture.h"

namespace Tupla
{
    class Texture2D final : public Asset
    {
    public:
        explicit Texture2D(const int aId): Asset(aId) {}

        operator Ref<Texture>() { return m_Texture; }

        void DeserializeAsset(const std::string& aFilePath) override;
        void SerializeAsset(std::vector<std::byte>& outResult) override;

        void DeserializeAssetPacked(const std::byte* data, u64 dataSize) override;
        void SerializeAssetPacked(std::vector<std::byte>& outResult) override;

    private:
        Ref<Texture> m_Texture;
    };
}
