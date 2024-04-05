#pragma once
#include <bitset>

using HOTEntity = unsigned;
using ComponentType = uint8_t;
using SystemType = uint8_t;

constexpr HOTEntity MAX_ENTITIES = 5000;
constexpr ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

template<typename T>
struct RUNTIME_STATIC_STORAGE
{
	static inline uint8_t mIndex = -1;
};