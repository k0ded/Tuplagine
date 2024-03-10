#pragma once
#include "Tupla/Utils/Hashing.h"

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

        u64 GetPropertyBlockSize();
        virtual void CopyPropertyBlock(void*) = 0;

    protected:
        virtual void SetInternal(u64 offset, void* src, u64 srcSize) = 0;
        virtual u64 GetOffsetFromHash(u32 hash) = 0;
    };
}
