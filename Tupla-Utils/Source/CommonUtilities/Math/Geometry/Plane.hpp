#pragma once
#include "../Vector/Vector.h"

namespace CommonUtilities
{
    template <typename T>
    class Plane
    {
    public:
        // Default constructor.
        Plane();
        // Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2 - aPoint0).
        Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
        
        // Constructor taking a point and a normal.
        Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal);
        
        // Init the plane with three points, the same as the constructor above.
        void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2);
        
        // Init the plane with a point and a normal, the same as the constructor above.
        void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal);

        // Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
        bool IsInside(const Vector3<T>& aPosition) const;

        const Vector3<T>& GetPoint() const;
        
        // Returns the normal of the plane.
        const Vector3<T>& GetNormal() const;

    private:
        Vector3<T> myPoint;
        Vector3<T> myNormal;
    };

    template <typename T>
    Plane<T>::Plane() = default;

    template <typename T>
    Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
    {
        InitWith3Points(aPoint0, aPoint1, aPoint2);
    }

    template <typename T>
    Plane<T>::Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
    {
        InitWithPointAndNormal(aPoint0, aNormal);
    }

    template <typename T>
    void Plane<T>::InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
    {
        myPoint = aPoint0;
        myNormal = (aPoint1 - aPoint0).Cross(aPoint2 - aPoint0).GetNormalized();
    }

    template <typename T>
    void Plane<T>::InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
    {
        myPoint = aPoint;
        myNormal = aNormal;
    }

    template <typename T>
    bool Plane<T>::IsInside(const Vector3<T>& aPosition) const
    {
        return (aPosition - myPoint).Dot(myNormal) <= 0;
    }

    template <typename T>
    const Vector3<T>& Plane<T>::GetPoint() const
    {
        return myPoint;
    }

    template <typename T>
    const Vector3<T>& Plane<T>::GetNormal() const
    {
        return myNormal;
    }
}
