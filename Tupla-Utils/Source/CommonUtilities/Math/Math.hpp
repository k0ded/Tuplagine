#pragma once

#include <assert.h>
#include <functional>

#include "Vector/Vector2.hpp"

namespace CommonUtilities
{
	namespace Math
	{
		constexpr float PI = 3.14159265358979323846264f;
		constexpr float TAU = PI * 2.0f;
		constexpr float SQRT2 = 1.41421356f;
		constexpr float INVSQRT2 = 1 / SQRT2;
		constexpr float DEG2RAD = PI / 180;
		constexpr float RAD2DEG = 180 / PI;

		template<typename T>
		inline T Min(T aA, T aB)
		{
			return aA < aB ? aA : aB;
		}

		template<typename T>
		inline T Max(T aA, T aB)
		{
			return aA > aB ? aA : aB;
		}

		template<typename T>
		inline T Abs(T aValue)
		{
			return aValue < 0 ? -aValue : aValue;
		}

		template<typename T>
		inline T Sign(T aValue)
		{
			return aValue < 0 ? (T)-1 : (T)1;
		}

		template<typename T>
		inline T SignOrZero(T aValue)
		{
			if (aValue == (T)0)
				return 0;
			else
				return aValue < 0 ? (T)-1 : (T)1;
		}

		template<typename T>
		inline T Clamp(T aValue, T aMin, T aMax)
		{
			assert(aMin <= aMax);

			if (aValue < aMin)
				return aMin;
			else if (aValue > aMax)
				return aMax;
			else
				return aValue;
		}

		template<typename T>
		inline T Lerp(T aA, T aB, float aT)
		{
			return aA + static_cast<T>((aB - aA) * aT);
		}

		template<typename T>
		inline void Swap(T &aA, T &aB)
		{
			T temp = aA;
			aA = aB;
			aB = temp;
		}

		template<typename T>
		inline bool Compare(T aX, T aY, T aEpsilon)
		{
			return abs(aX - aY) <= aEpsilon * Max(T(1), Max(abs(aX), abs(aY)));
		}

		template<typename T>
		inline bool Compare(Vector2<T> aVec1, Vector2<T> aVec2, T aEpsilon)
		{
			return Compare(aVec1.x, aVec2.x, aEpsilon) && Compare(aVec1.y, aVec2.y, aEpsilon);
		}
		
		template <typename T>
		constexpr T Epsilon()
		{
			static_assert(std::numeric_limits<T>::is_iec559, "'epsilon' only accepts floating-point inputs");
			return std::numeric_limits<T>::epsilon();
		}

		float SmoothTowards(float aCurrent, float aTarget, float& aRefCurrentVelocity, float aSmoothTime, float aMaxSpeed = 9999999.0f);
	}
	
	namespace Nerd = Math;

	namespace Container
	{
		template <typename Iter, typename T>
		bool All(Iter aBegin, Iter aEnd, std::function<bool(const T&)> aPredicate)
		{
			while (aBegin != aEnd)
			{
				const bool result = aPredicate(*aBegin);
				++aBegin;
				
				if(!result)
				{
					return false;
				}
			}

			return true;
		}

		template <typename Iter, typename T>
		bool Any(Iter aBegin, Iter aEnd, std::function<bool(const T&)> aPredicate)
		{
			while (aBegin != aEnd)
			{
				const bool result = aPredicate(*aBegin);
				++aBegin;
				
				if(result)
				{
					return true;
				}
			}

			return false;
		}

		template <typename Iter, typename T>
		bool None(Iter aBegin, Iter aEnd, std::function<bool(const T&)> aPredicate)
		{
			while (aBegin != aEnd)
			{
				const bool result = aPredicate(*aBegin);
				++aBegin;
				
				if(result)
				{
					return false;
				}
			}

			return true;
		}

		template <typename Container>
		bool All(Container aContainer)
		{
			static_assert(std::is_same_v<Container::value_type, bool>);
			return All(aContainer.begin(), aContainer.end(), std::function([](const bool& aBool) { return aBool; }));
		}
		
		template <typename Container>
		bool Any(Container aContainer)
		{
			static_assert(std::is_same_v<Container::value_type, bool>);
			return Any(aContainer.begin(), aContainer.end(), std::function([](const bool& aBool) { return aBool; }));
		}
		
		template <typename Container>
		bool None(Container aContainer)
		{
			static_assert(std::is_same_v<Container::value_type, bool>);
			return None(aContainer.begin(), aContainer.end(), std::function([](const bool& aBool) { return aBool; }));
		}
	}

	namespace Bob = Container;
}
namespace Nerd = CommonUtilities::Math;
namespace CU = CommonUtilities;