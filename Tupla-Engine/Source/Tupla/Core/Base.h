#pragma once

#include <memory>
#include <CommonUtilities/g_types.h>
#include "Tupla/Utils/Hashing.h"
#include <combaseapi.h>
#include <format>
#include <set>

#define T_EXPAND_MACRO(x) x
#define T_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

#define T_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Tupla
{
    inline std::string GetAsString(const GUID& guid)
    {
        char guid_string[37]; // 32 hex chars + 4 hyphens + null terminator
        snprintf(
            guid_string, sizeof(guid_string),
            "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2],
            guid.Data4[3], guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
        return guid_string;
    }
    
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

#ifdef _WIN32
    #ifndef _WIN64
        #error "x86 not supported!"
    #endif
    #define TUP_PLATFORM_WINDOWS
#endif

#ifdef __linux__
    #define TUP_PLATFORM_LINUX
#endif

#ifdef _DEBUG
    #ifdef TUP_PLATFORM_WINDOWS
        #define TUP_DEBUGBREAK() __debugbreak()
    #endif

    #ifdef TUP_PLATFORM_LINUX
    #include <csignal>
    #define TUP_DEBUGBREAK() raise(SIGTRAP)
    #endif
#else
#define TUP_DEBUGBREAK()
#endif 

#define ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); abort(); } }

#ifdef _DEBUG
#define DEBUG_ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); TUP_DEBUGBREAK(); } }
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[VULKAN ERROR = {0}] {1}", VK_ERROR_STRING((res)), __VA_ARGS__); TUP_DEBUGBREAK(); } }
#else
#define DEBUG_ASSERT(condition, ...)
#define DEBUG_VK(res, ...)
#endif