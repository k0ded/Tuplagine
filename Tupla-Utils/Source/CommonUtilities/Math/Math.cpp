#include "Math.hpp"
#include "../Time/Time.h"

float CommonUtilities::Math::SmoothTowards(float aCurrent, float aTarget, float& aRefCurrentVelocity, float aSmoothTime, float aMaxSpeed)
#if 1
{
	// Taken from Game Programming Gems 4 Chapter 1.10
	// Originally discovered through (and maybe inspired by Unity's changes) BUT COPIED THROUGH THE BOOK!
	// No license broken! (hopefully, I'm not a lawyer and I only skimmed the license)
	const float deltaTime = Time::DeltaTime();

	const float omega = 2.0f / aSmoothTime;
	const float x = omega * deltaTime;
	const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

	const float change = Nerd::Clamp(aCurrent - aTarget, -aMaxSpeed * aSmoothTime, aMaxSpeed * aSmoothTime);

	const float temp = (aRefCurrentVelocity + omega * change) * deltaTime;
	aRefCurrentVelocity = (aRefCurrentVelocity - omega * temp) * exp;

	float result = (aCurrent - change) + (change + temp) * exp;
	if (change > 0 == result > aTarget) // TODO: Rewrite this
	{
		result = aTarget;
		aRefCurrentVelocity = (aTarget - aCurrent) / deltaTime;
	}

	return result;
}
#else
{
	// Taken from Game Programming Gems 4 Chapter 1.10
	// Originally discovered through (and maybe inspired by Unity's changes) BUT COPIED THROUGH THE BOOK!
	// No license broken! (hopefully, I'm not a lawyer and I only skimmed the license)
	const float deltaTime = Time::DeltaTime();

	const float omega = 2.0f / aSmoothTime;
	const float x = omega * deltaTime;
	const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

	const float change = Nerd::Clamp(aTarget - aCurrent, -aMaxSpeed * aSmoothTime, aMaxSpeed * aSmoothTime);

	const float temp = (aRefCurrentVelocity + omega * change) * deltaTime;
	aRefCurrentVelocity = (aRefCurrentVelocity + omega * temp) * exp;

	float result = aCurrent + (change + temp) * exp;
	if (change > 0 == result > aTarget) // TODO: Rewrite this
	{
		result = aTarget;
		aRefCurrentVelocity = (aTarget - aCurrent) / deltaTime;
	}

	return result;
}
#endif