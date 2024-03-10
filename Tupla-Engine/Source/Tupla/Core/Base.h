#pragma once

#include <memory>
#include <CommonUtilities/g_types.h>
#include <combaseapi.h>
#include <format>

#define T_EXPAND_MACRO(x) x
#define T_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

#define T_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Tupla
{
    /*inline std::string GetAsString(GUID& guid)
    {
        // From JustinB at https://stackoverflow.com/questions/1672677/print-a-guid-variable
        return std::format("GUID = {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
                guid.Data1, guid.Data2, guid.Data3, 
                guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
                guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    }*/
    
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}