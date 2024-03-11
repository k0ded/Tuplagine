#pragma once
#include "Tupla/Utils/Hashing.h"
#include "Tupla/Core/Base.h"
#include "Shader.h"

namespace Tupla
{
    class Material
    {
    public:
        virtual ~Material() = default;
    
        template<typename T>
        void Set(const u32 hashed, const T& value) { SetInternal(GetOffsetFromHash(hashed), &value, sizeof(T)); }
        template<const char* Name, typename T>
        void Set(const T& value) { return Set<T>(HASH_STR(Name), value); }

        virtual void SetFragmentShader(Ref<Shader> shader) = 0;
        virtual void SetVertexShader(Ref<Shader> shader) = 0;

        u64 GetPropertyBlockSize() const { return m_PropertyBlockSize; }
    protected:
        virtual void SetInternal(u64 offset, void* src, u64 srcSize) = 0;
        virtual u64 GetOffsetFromHash(u32 hash) = 0;

        u64 m_PropertyBlockSize{};
    };
}
