#pragma once
#include "../Vector/Vector3.hpp"

namespace CommonUtilities
{
    template <typename T>
    class Ray
    {
    public:
        // Default constructor: there is no ray, the origin and direction are the
        // zero vector.
        Ray() = default;
        
        // Copy constructor.
        Ray(const Ray& aRay);
        
        // Constructor that takes origin and direction to define the ray
        Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);
        
        // Init the ray with two points.
        void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint);
        
        // Init the ray with an origin and a direction.
        void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection);

        const Vector3<T>& GetOrigin() const;
        const Vector3<T>& GetDirection() const;

    private:
        Vector3<T> myOrigin;
        Vector3<T> myDirection;
    };

    template <typename T>
    Ray<T>::Ray(const Ray& aRay)
    {
        myOrigin = aRay.myOrigin;
        myDirection = aRay.myDirection;
    }

    template <typename T>
    Ray<T>::Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
    {
        InitWithOriginAndDirection(aOrigin, aDirection);
    }

    template <typename T>
    void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
    {
        myOrigin = aOrigin;
        myDirection = (aPoint - aOrigin).GetNormalized();
    }

    template <typename T>
    void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
    {
        myOrigin = aOrigin;
        myDirection = aDirection;
    }

    template <typename T>
    const Vector3<T>& Ray<T>::GetOrigin() const
    {
        return myOrigin;
    }

    template <typename T>
    const Vector3<T>& Ray<T>::GetDirection() const
    {
        return myDirection;
    }
}
