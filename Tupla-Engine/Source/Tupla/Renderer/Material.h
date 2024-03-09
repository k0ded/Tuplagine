#pragma once
#include "Tupla/Utils/Hashing.h"

class Material
{
public:
    template<typename T>
    void Set(const u64 hashed, const T& value) { SetInternal(GetOffsetFromHash(hashed), &value, sizeof(T)); }
    template<const char* Name, typename T>
    void Set(const T& value) { return Set<T>(HASH_STR(Name), value); }

    u64 GetPropertyBlockSize();
    virtual void CopyPropertyBlock(void*) = 0;

protected:
    virtual void SetInternal(u64 offset, void* src, u64 srcSize) = 0;
    virtual u64 GetOffsetFromHash(u64 hash) = 0;
};
