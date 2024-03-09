#pragma once

namespace CommonUtilities
{
	namespace Random
	{
		int RandomInt(int aMin, int aMax);

		int RandomSignInt();
		float RandomSignFloat();
		float RandomFloat(float aMin, float aMax);
		bool RandomChance(float aChancePercent);
	}
}