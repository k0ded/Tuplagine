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
        std::string SerializeAsset() override;
        void DeserializeAssetPacked(const std::string& data) override;
        std::string SerializeAssetPacked() override;

    private:
        Ref<Texture> m_Texture;
    };
}
