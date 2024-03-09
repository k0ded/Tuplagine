#pragma once
#include "../Vector/Vector.h"

namespace CommonUtilities
{
    template<typename T>
    class Line
    {
    public:
        // Default constructor: there is no line, the normal is the zero vector.
        Line();
        
        // Copy constructor.
        Line(const Line& aLine);
        
        // Constructor that takes two points that define the line, the direction is aPoint1 - aPoint0.
        Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
        
        // Init the line with two points, the same as the constructor above.
        void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
        
        // Init the line with a point and a direction.
        void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);
        
        // Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
        bool IsInside(const Vector2<T>& aPosition) const;
        
        // Returns the direction of the line.
        const Vector2<T>& GetDirection() const;
        
        // Returns the normal of the line, which is (-direction.y, direction.x).
        Vector2<T> GetNormal() const;

    private:
        Vector2<T> myPoint;
        Vector2<T> myDirection;
    };

    template <typename T>
    Line<T>::Line() = default;

    template <typename T>
    Line<T>::Line(const Line& aLine) = default;

    template <typename T>
    Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
    {
        InitWith2Points(aPoint0, aPoint1);
    }

    template <typename T>
    void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
    {
        myPoint = aPoint0;
        myDirection = (aPoint1 - aPoint0).GetNormalized();
    }

    template <typename T>
    void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
    {
        myPoint = aPoint;
        myDirection = aDirection;
    }

    template <typename T>
    bool Line<T>::IsInside(const Vector2<T>& aPosition) const
    {
        return (aPosition - myPoint).Dot(GetNormal()) <= 0;
    }

    template <typename T>
    const Vector2<T>& Line<T>::GetDirection() const
    {
        return myDirection;
    }

    template <typename T>
    Vector2<T> Line<T>::GetNormal() const
    {
        return { -myDirection.y, myDirection.x };
    }
}
