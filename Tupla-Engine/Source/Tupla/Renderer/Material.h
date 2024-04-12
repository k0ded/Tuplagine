#pragma once
#include "Tupla/Core/Base.h"
#include <CommonUtilities/Math/Matrices/Matrix4x4.hpp>
#include <CommonUtilities/Math/Vector/Vector3.hpp>

#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"

namespace Tupla
{
    struct PerObject
    {
        CU::Matrix4x4<float> Transform;
        CU::Vector3f LightVector;
    };

    struct PerFrame
    {
        CU::Matrix4x4<float> worldToClip;
        CU::Matrix4x4<float> worldToCamera;
    };

    struct BufferBinding
    {
        ShaderStage Stage;
        Ref<Buffer> ConstBuffer;
    };

    enum class VertexBufferType : u64 // DONT FORGET TO UPDATE the DX11Material constructor if you change anything here!
    {
        PerObject,
        PerFrame,
        Count
    };

    class Material
    {
    public:
        virtual ~Material() = default;

        virtual void SetShader(Ref<Shader> shader) = 0;
        virtual void AttachImage(Ref<Texture> texture) = 0;
        virtual void AttachBuffer(Ref<Buffer> buffer, ShaderStageSlot stage) = 0;
        virtual Ref<Buffer> GetBuffer(size_t slot, ShaderStageSlot stage) = 0;
        virtual bool BindMaterial() const = 0;
        virtual u64 GetId() const = 0;

        static Ref<Buffer> GetGlobalBuffer(size_t slot, ShaderStageSlot stage)
        {
            return globalBuffers[static_cast<size_t>(stage)][slot];
        }

    protected:
        static inline std::array<std::vector<Ref<Buffer>>, static_cast<u64>(ShaderStageSlot::Count)> globalBuffers{};
    };
}
