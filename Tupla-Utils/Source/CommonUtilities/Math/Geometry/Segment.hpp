#pragma once
#include "../Vector/Vector.h"

namespace CommonUtilities
{
    template<typename T>
    class Segment
    {
    public:
        // Default constructor: there is no Segment, the normal is the zero vector.
        Segment();
        
        // Copy constructor.
        Segment(const Segment& aSegment);
        
        // Constructor that takes two points that define the Segment.
        Segment(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
        
        // Init the Segment with two points, the same as the constructor above.
        void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
        
        // Returns whether a point is inside the Segment: it is inside when the point is on the Segment or on the side the normal is pointing away from.
        bool IsInside(const Vector2<T>& aPosition) const;
        
        const Vector2<T>& GetStart() const;
        const Vector2<T>& GetEnd() const;
        
        // Returns the normal of the Segment, which is (-direction.y, direction.x).
        Vector2<T> GetNormal() const;
        T GetLengthSqr() const;

    private:
        Vector2<T> myFrom;
        Vector2<T> myTo;
    };

    template <typename T>
    Segment<T>::Segment() = default;

    template <typename T>
    Segment<T>::Segment(const Segment& aSegment) = default;

    template <typename T>
    Segment<T>::Segment(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
    {
        InitWith2Points(aPoint0, aPoint1);
    }

    template <typename T>
    void Segment<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
    {
        myFrom = aPoint0;
        myTo = aPoint1;
    }

    template <typename T>
    bool Segment<T>::IsInside(const Vector2<T>& aPosition) const
    {
        return (aPosition - myFrom).Dot(GetNormal()) <= 0;
    }

    template <typename T>
    const Vector2<T>& Segment<T>::GetStart() const
    {
        return myFrom;
    }

    template <typename T>
    const Vector2<T>& Segment<T>::GetEnd() const
    {
        return myTo;
    }

    template <typename T>
    Vector2<T> Segment<T>::GetNormal() const
    {
        return Vector2<T>{ -myTo.y, myTo.x }.GetNormalized();
    }

    template <typename T>
    T Segment<T>::GetLengthSqr() const
    {
        return (myTo - myFrom).LengthSqr();
    }
}
