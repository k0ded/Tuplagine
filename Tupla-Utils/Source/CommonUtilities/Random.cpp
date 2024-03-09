#include <random>
#include "Random.h"

// TODO: This might result in weirdness if it produces the exact same sequence on different threads..?
static thread_local std::mt19937 locRNG = std::mt19937{ std::random_device()() };

namespace CommonUtilities
{
	namespace Random
	{
		int RandomInt(int aMin, int aMax)
		{
			return std::uniform_int_distribution<int>(aMin, aMax)(locRNG);
		}

		int RandomSignInt()
		{
			return RandomInt(0, 1) * 2 - 1;
		}

		float RandomSignFloat()
		{
			return RandomInt(0, 1) * 2.0f - 1.0f;
		}

		float RandomFloat(float aMin, float aMax)
		{
			return std::uniform_real_distribution<float>(aMin, aMax)(locRNG);
		}

		bool RandomChance(float aChancePercent)
		{
			return RandomFloat(0.0f, 1.0f) < aChancePercent;
		}
	}
}