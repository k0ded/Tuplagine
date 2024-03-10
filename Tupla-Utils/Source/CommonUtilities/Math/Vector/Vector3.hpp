#pragma once
#include <complex>

namespace CommonUtilities
{
    template <class T>
    class Matrix3x3;

    template <class T>
    class Vector2;

    template <class T>
    class Vector4;

    template <class T = float>
    class Vector3
    {
    public:
        T x = {};
        T y = {};
        T z = {};
        //Creates a null-vector
        Vector3() = default;
        //Creates a vector (aX, aY, aZ)
        Vector3(const T& aX, const T& aY, const T& aZ);
        //Creates a vector (aVector.x, aVector.y, z)
        Vector3(Vector2<T> aVector, T z = 0);
        //Copy constructor (compiler generated)
        Vector3(const Vector3& aVector) = default;
        //Destructor (compiler generated)
        ~Vector3() = default;
        //Explicit Type operator, create a different vector with the same values.
        //Example creates a Tga::Vector3<T> from this CommonUtillities::Vector3<T>
        template <class OtherVectorClass>
        operator OtherVectorClass() const;

        //Assignment operator (compiler generated)
        Vector3& operator=(const Vector3& aVector3) = default;
        //Returns a negated copy of the vector
        Vector3 operator-() const;

        //Returns the dot product of this and aVector
        T Dot(const Vector3& aVector) const;
        //Returns the cross product of this and aVector
        Vector3 Cross(const Vector3& aVector) const;
        //Returns the squared length of the vector
        T LengthSqr() const;
        //Returns the length of the vector
        T Length() const;
        //Returns a normalized copy of this vector
        Vector3 GetNormalized() const;
        //Normalizes the vector
        void Normalize();

        Vector4<T> ToVec4(T aValue = 0) const;
        Vector2<T> ToVec2() const;
    };

    template <class T>
    Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
        : x {aX}, y { aY }, z {aZ}
    {
    }

    template<class T>
    Vector3<T>::Vector3(Vector2<T> aVector, T z)
        : x {aVector.x}, y { aVector.y }, z { z }
    {
    }

    template <class T>
    Vector3<T> Vector3<T>::operator-() const
    {
        return *this * T(-1);
    }

    template <class T>
    T Vector3<T>::LengthSqr() const
    {
        return x*x + y*y + z*z;
    }

    template <class T>
    T Vector3<T>::Length() const
    {
        return T(std::sqrt(LengthSqr()));
    }

    template <class T>
    Vector3<T> Vector3<T>::GetNormalized() const
    {
        T len = Length();

        if(len == 0)
        {
            return { T(0), T(0), T(0) };
        }
        
        return *this * (1 / len);
    }

    template <class T>
    void Vector3<T>::Normalize()
    {
        *this = GetNormalized();
    }

    template <class T>
    Vector4<T> Vector3<T>::ToVec4(T aValue) const
    {
         return { x, y, z, aValue };
    }

    template <class T>
    Vector2<T> Vector3<T>::ToVec2() const
    {
        return { x, y };
    }

    template <class T>
    T Vector3<T>::Dot(const Vector3& aVector) const
    {
        return x * aVector.x + y * aVector.y + z * aVector.z;
    }

    template <class T>
    Vector3<T> Vector3<T>::Cross(const Vector3& aVector) const
    {
        Vector3 vec = {};
        vec.x = (y * aVector.z) - (z * aVector.y);
        vec.y = (z * aVector.x) - (x * aVector.z);
        vec.z = (x * aVector.y) - (y * aVector.x);
        return vec;
    }

    template<class T>
    bool operator==(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
    {
        return aVector0.x == aVector1.x && aVector0.y == aVector1.y && aVector0.z == aVector1.z;
    }

    //Returns the vector sum of aVector0 and aVector1
    template <class T>
    Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
    {
        return Vector3<T>((aVector0).x + (aVector1).x, (aVector0).y + (aVector1).y, (aVector0).z + (aVector1).z);
    }

    //Returns the vector difference of aVector0 and aVector1
    template <class T>
    Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
    {
        return Vector3<T>((aVector0).x - (aVector1).x, (aVector0).y - (aVector1).y, (aVector0).z - (aVector1).z);
    }

    //Returns the vector aVector1 component-multiplied by aVector1
    template <class T>
    Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T> aVector1)
    {
        return Vector3<T>((aVector0).x * (aVector1).x, (aVector0).y * (aVector1).y, (aVector0).z * (aVector1).z);
    }

    //Returns the vector aVector multiplied by the scalar aScalar
    template <class T>
    Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
    {
        return Vector3<T>((aVector).x * (aScalar), (aVector).y * (aScalar), (aVector).z * (aScalar));
    }

    //Returns the vector aVector multiplied by the scalar aScalar
    template <class T>
    Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
    {
        return Vector3<T>((aVector).x * (aScalar), (aVector).y * (aScalar), (aVector).z * (aScalar));
    }

    //Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
    template <class T>
    Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
    {
        return Vector3<T>((aVector).x / (aScalar), (aVector).y / (aScalar), (aVector).z / (aScalar));
    }
    
    //Returns the vector aVector divided by the vector aVectorB
    template <class T>
    Vector3<T> operator/(const Vector3<T>& aVector, const Vector3<T>& aVectorB)
    {
        return Vector3<T>(aVector.x / aVectorB.x, aVector.y / aVectorB.y, aVector.z / aVectorB.z);
    }

    template <class T>
    Vector3<T> operator/(const T& aScalar, const Vector3<T>& aVector)
    {
        return Vector3<T>((aScalar) / (aVector).x, (aScalar) / (aVector).y, (aScalar) / (aVector).z);
    }

    //Equivalent to setting aVector0 to (aVector0 + aVector1)
    template <class T>
    void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
    {
        aVector0 = aVector0 + aVector1;
    }

    //Equivalent to setting aVector0 to (aVector0 - aVector1)
    template <class T>
    void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
    {
        aVector0 = aVector0 - aVector1;
    }

    //Equivalent to setting aVector to (aVector * aScalar)
    template <class T>
    void operator*=(Vector3<T>& aVector, const T& aScalar)
    {
        aVector = aVector * aScalar;
    }

    //Equivalent to setting aVector to (aVector / aScalar)
    template <class T>
    void operator/=(Vector3<T>& aVector, const T& aScalar)
    {
        aVector = aVector / aScalar;
    }

    typedef Vector3<unsigned>   Vector3ui;
    typedef Vector3<double>     Vector3d;
    typedef Vector3<float>      Vector3f;
    typedef Vector3<int>        Vector3i;
}

namespace CU = CommonUtilities;