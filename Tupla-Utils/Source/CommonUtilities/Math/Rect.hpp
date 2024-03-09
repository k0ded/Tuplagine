#pragma once

#include "Vector\Vector2.hpp"
#include "Math.hpp"

namespace CommonUtilities
{
	template<typename T>
	struct Rectangle
	{
		template <typename T> using Vector2 = Vector2<T>;

#pragma warning(disable: 4201)
		union
		{
			Vector2<T> position;
			struct { T x, y; };
		};
		union
		{
			Vector2<T> size;
			struct { T width, height; };
		};
#pragma warning(default: 4201)

		Rectangle() : position{ 0, 0 }, size{ 0, 0 } {}

		template<typename ValueType>
		Rectangle(ValueType aX, ValueType aY, ValueType aWidth, ValueType aHeight) : position{ static_cast<T>(aX), static_cast<T>(aY) }, size{ static_cast<T>(aWidth), static_cast<T>(aHeight) } {}

		template<typename ValueType>
		Rectangle(Vector2<ValueType> aPosition, Vector2<ValueType> aSize) : position{ aPosition }, size{ aSize } {}
		~Rectangle() {}

		Rectangle(const Rectangle<T>& aRect) : position { aRect.position }, size{ aRect.size }
		{
		}

		__forceinline T Left()		const { return position.x; }
		__forceinline T Right()		const { return position.x + width; }
		__forceinline T Top()		const { return position.y + height; }
		__forceinline T Bottom()	const { return position.y; }

		__forceinline Vector2<T> TopRight() const { return position + Vector2<T>(width, height); }
		__forceinline Vector2<T> TopLeft() const { return position + Vector2<T>(0, height); }
		__forceinline Vector2<T> BottomRight() const { return position + Vector2<T>(width, 0); }
		__forceinline Vector2<T> BottomLeft() const { return position; }
		__forceinline Vector2<T> Center() const { return position + size / T(2); } // Use division instead of multiplication by 0.5f because it works with both int and float
		__forceinline Vector2<T> TopCenter() const { return Vector2<T>(x + width / T(2), y + height); }
		__forceinline Vector2<T> BottomCenter() const { return Vector2<T>(x + width / T(2), y); }
		__forceinline Vector2<T> LeftCenter() const { return Vector2<T>(x, y + height / T(2)); }
		__forceinline Vector2<T> RightCenter() const { return Vector2<T>(x + width, y + height / T(2)); }

		__forceinline Rectangle<T> Centered() const { return Rectangle(static_cast<T>(x - width / T(2)), static_cast<T>(y - height / T(2)), width, height); }

		__forceinline Vector2<T> operator*(Vector2<T> aVector) { return aVector * size + position; }
		__forceinline Vector2<T> operator*(Vector2<T> aVector) const { return aVector * size + position; }

		template<typename ValueType>
		bool Intersects(const Rectangle<ValueType> &aOtherRect) const
		{
			// If one rect is to the left of, to the right of, above or below, then there is no overlap
			bool noOverlap =
				Right() < aOtherRect.Left() || Left() > aOtherRect.Right()
				|| Bottom() > aOtherRect.Top() || Top() < aOtherRect.Bottom();

			return !noOverlap;
		}

		template<typename ValueType>
		bool Contains(Vector2<ValueType> aPoint) const
		{
			return aPoint.x >= Left() && aPoint.x <= Right()
				&& aPoint.y <= Top() && aPoint.y >= Bottom();
		}

		template<typename ValueType>
		bool Contains(Rectangle<ValueType> &aRect)
		{
			return Left() >= aRect.Left() && aRect.Right() <= Right()
				&& Bottom() >= aRect.Bottom() && aRect.Top() <= Top();
		}

		void MoveLeftEdge(T aDistance)
		{
			*this = FromPoints(position + Vec2f(aDistance, 0.0f), position + size);
		}

		void MoveRightEdge(T aDistance)
		{
			*this = FromPoints(position, position + size + Vec2f(aDistance, 0.0f));
		}

		void MoveTopEdge(T aDistance)
		{
			*this = FromPoints(position, position + size + Vec2f(0.0f, aDistance));
		}

		void MoveBottomEdge(T aDistance)
		{
			*this = FromPoints(position + Vec2f(0.0f, aDistance), position + size);
		}

		static Rectangle FromPoints(Vector2<T> aP1, Vector2<T> aP2)
		{
			namespace Math = CommonUtilities::Math;

			if (aP1.x < aP2.x)
			{
				float temp = aP1.x;
				aP1.x = aP2.x;
				aP2.x = temp;
			}

			if (aP1.y < aP2.y)
			{
				float temp = aP1.y;
				aP1.y = aP2.y;
				aP2.y = temp;
			}

			return FromMinMax(aP1, aP2);
		}

		__forceinline static Rectangle FromMinMax(Vector2<T> min, Vector2<T> aMax)
		{
			return Rectangle(min, aMax - min);
		}

		static bool TryGetIntersection(const Rectangle &aRect1, const Rectangle &aRect2, Rectangle &aOutIntersection)
		{
			namespace Math = CommonUtilities::Math;

			// Lazy... https://stackoverflow.com/a/19754915
			float intersectionMinX = Math::Max(aRect1.x, aRect2.x);
			float intersectionMinY = Math::Max(aRect1.y, aRect2.y);
			float intersectionMaxX = Math::Min(aRect1.Right(), aRect2.Right());
			float intersectionMaxY = Math::Min(aRect1.Top(), aRect2.Top());

			if (intersectionMinX >= intersectionMaxX || intersectionMinY >= intersectionMaxY)
			{
				return false;
			}
			else
			{
				aOutIntersection = Rectangle::FromMinMax({ intersectionMinX, intersectionMinY }, { intersectionMaxX, intersectionMaxY });
				return true;
			}
		}
	};

	inline void from_json(const nlohmann::json& aJson, Rectangle<float>& aRectangle)
	{
		const float minX = aJson["xMin"];
		const float maxX = aJson["xMax"];
		const float minY = aJson["yMin"];
		const float maxY = aJson["yMax"];
		
		aRectangle.size = { maxX - minX, maxY - minY };
		aRectangle.position = { minX, minY };
	}

	using Recti = Rectangle<int>;
	using Rectf = Rectangle<float>;
}

namespace CU = CommonUtilities;