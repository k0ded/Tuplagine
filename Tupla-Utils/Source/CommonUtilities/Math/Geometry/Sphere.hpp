#pragma once
#include "../Vector/Vector3.hpp"

namespace CommonUtilities
{
    template <typename T>
    class Sphere
    {
    public:
        // Default constructor: there is no sphere, the radius is zero and the position is
        // the zero vector.
        Sphere();

        // Copy constructor.
        Sphere(const Sphere& aSphere);

        // Constructor that takes the center position and radius of the sphere.
        Sphere(const Vector3<T>& aCenter, T aRadius);

        // Init the sphere with a center and a radius, the same as the constructor above.
        void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius);

        // Returns whether a point is inside the sphere: it is inside when the point is on the
        // sphere surface or inside of the sphere.
        bool IsInside(const Vector3<T>& aPosition) const;

        const Vector3<T>& GetCenter() const;
        const T& GetRadius() const;
        
    private:
        Vector3<T> myCenter;
        T myRadius;
    };

    template <typename T>
    Sphere<T>::Sphere() = default;

    template <typename T>
    Sphere<T>::Sphere(const Sphere& aSphere)
    {
        myCenter = aSphere.myCenter;
        myRadius = aSphere.myRadius;
    }

    template <typename T>
    Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius)
    {
        InitWithCenterAndRadius(aCenter, aRadius);
    }

    template <typename T>
    void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
    {
        myCenter = aCenter;
        myRadius = aRadius;
    }

    template <typename T>
    bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
    {
         return (aPosition - myCenter).LengthSqr() <= myRadius * myRadius;
    }

    template <typename T>
    const Vector3<T>& Sphere<T>::GetCenter() const
    {
        return myCenter;
    }

    template <typename T>
    const T& Sphere<T>::GetRadius() const
    {
        return myRadius;
    }
}
