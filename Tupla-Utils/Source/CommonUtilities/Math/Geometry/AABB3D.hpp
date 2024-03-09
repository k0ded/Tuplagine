#pragma once
#include "../Vector/Vector3.hpp"

namespace CommonUtilities
{
    template <typename T>
    class AABB3D
    {
    public:
        // Default constructor: there is no AABB, both min and max points are the zero vector.
        AABB3D() = default;

        // Copy constructor.
        AABB3D(const AABB3D& aAABB3D);

        // Constructor taking the positions of the minimum and maximum corners.
        AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);

        // Init the AABB with the positions of the minimum and maximum corners, same as
        // the constructor above.
        void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);

        // Returns whether a point is inside the AABB: it is inside when the point is on any
        // of the AABB's sides or inside of the AABB.
        bool IsInside(const Vector3<T>& aPosition) const;

        const Vector3<T>& GetMin() const;
        const Vector3<T>& GetMax() const;

        Vector3<T> GetSize() const;
        Vector3<T> GetHalf() const;
        AABB3D Combine(const AABB3D& aAabb3D) const;
        Vector3<T> GetPosition() const;

    private:
        Vector3<T> myMin;
        Vector3<T> myMax;
    };

    template <typename T>
    inline AABB3D<T>::AABB3D(const AABB3D& aAABB3D)
    {
        myMin = aAABB3D.myMin;
        myMax = aAABB3D.myMax;
    }

    template <typename T>
    inline AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
    {
        InitWithMinAndMax(aMin, aMax);
    }

    template <typename T>
    inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
    {
        myMin = aMin;
        myMax = aMax;
    }

    template <typename T>
    inline bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
    {
        const bool isInsideX = aPosition.x >= myMin.x && aPosition.x <= myMax.x;
        const bool isInsideY = aPosition.y >= myMin.y && aPosition.y <= myMax.y;
        const bool isInsideZ = aPosition.z >= myMin.z && aPosition.z <= myMax.z;

        return isInsideX && isInsideY && isInsideZ;
    }

    template <typename T>
    inline const Vector3<T>& AABB3D<T>::GetMin() const
    {
        return myMin;
    }

    template <typename T>
    inline const Vector3<T>& AABB3D<T>::GetMax() const
    {
        return myMax;
    }

    template <typename T>
    inline Vector3<T> AABB3D<T>::GetSize() const
    {
        return myMax - myMin;
    }

    template<typename T>
    inline Vector3<T> AABB3D<T>::GetHalf() const
    {
        return GetSize() / T(2);
    }

    template <typename T>
    inline AABB3D<T> AABB3D<T>::Combine(const AABB3D& aAabb3D) const
    {
        Vector3<T> halfSize = aAabb3D.GetSize() / T(2);
        return { myMin - halfSize, myMax + halfSize };
    }

    template <typename T>
    inline Vector3<T> AABB3D<T>::GetPosition() const
    {
        Vector3<T> halfSize = GetSize() / T(2);
        return myMin + halfSize;
    }
}
