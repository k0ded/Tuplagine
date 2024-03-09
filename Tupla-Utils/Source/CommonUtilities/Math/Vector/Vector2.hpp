#pragma once
#include <assert.h>
#include <complex>

namespace CommonUtilities
{
    template <class T>
    class Vector3;
    
    template <class T = float>
    class Vector2
    {
    public:
        T x;
        T y;
        //Creates a null-vector
        Vector2();
        //Creates a vector (aX, aY)
        Vector2(const T& aX, const T& aY);
        //Copy constructor (compiler generated)
        Vector2(const Vector2& aVector);
        template <typename OtherVectorType>
        Vector2(const OtherVectorType& aVector);
        //Destructor (compiler generated)
        ~Vector2();
        //Explicit Type operator, create a different vector with the same values.
        //Example creates a Tga::Vector2<T> from this CommonUtillities::Vector2<T>
        template <class OtherVectorClass>
        operator OtherVectorClass() const;
        //Assignment operator (compiler generated)
        Vector2& operator=(const Vector2& aVector2) = default;
        //Returns a negated copy of the vector
        Vector2 operator-() const;
        T& operator[](size_t aIndex);
        const T& operator[](size_t aIndex) const;
        //Returns the dot product of this and aVector
        T Dot(const Vector2& aVector) const;
        //Returns the squared length of the vector
        T LengthSqr() const;
        //Returns the length of the vector
        T Length() const;
        
        static Vector2 ProjectOnPlane(Vector2 aUp, Vector2 aVector);
        //Returns a normalized copy of this vector
        Vector2 GetNormalized() const;
        Vector2 Sign() const;
        Vector2 Abs() const;
        
        //Normalizes the vector
        void Normalize();
        Vector3<T> ToVec3(T aZ = T(0));
    };

    template <typename T>
    float Cross(const Vector2<T>& aVector1, const Vector2<T>& aVector2)
    {
        return aVector1.x * aVector2.y - aVector1.y * aVector2.x;
    }
    
    template <typename T>
    Vector2<T> Cross(const Vector2<T>& aVector, float aScalar)
    {
        return { aScalar * aVector.y, -aScalar * aVector.x };
    }
    
    template <typename T>
    Vector2<T> Cross(float aScalar, const Vector2<T>& aVector)
    {
        return { -aScalar * aVector.y, aScalar * aVector.x };
    }

    template <class T>
    Vector2<T>::Vector2()
    {
        x = T(0);
        y = T(0);
    }

    template <class T>
    Vector2<T>::Vector2(const T& aX, const T& aY)
    {
        x = aX;
        y = aY;
    }

    template <class T>
    Vector2<T>::Vector2(const Vector2& aVector)
    {
        x = aVector.x;
        y = aVector.y;
    }


    template <class T>
    template <typename OtherVectorType>
    Vector2<T>::Vector2(const OtherVectorType& aVector)
        : x { T(aVector.x) }, y { T(aVector.y) }
    {
    }

    template <class T>
    Vector2<T>::~Vector2() = default;

    template <class T>
    template <class OtherVectorClass>
    Vector2<T>::operator OtherVectorClass() const
    {
        return { x, y };
    }

    template <class T>
    Vector2<T> Vector2<T>::operator-() const
    {
        return *this * T(-1);
    }

    template <class T>
    T& Vector2<T>::operator[](const size_t aIndex)
    {
        switch (aIndex)
        {
        case 0:
            return x;
        case 1:
            return y;
        }
        throw std::out_of_range("INDEX OUT OF RANGE!");
    }

    template <class T>
    const T& Vector2<T>::operator[](const size_t aIndex) const
    {
        switch (aIndex)
        {
        case 0:
            return x;
        case 1:
            return y;
        }
        throw std::out_of_range("INDEX OUT OF RANGE!");
    }

    template <class T>
    T Vector2<T>::LengthSqr() const
    {
        return x*x + y*y;
    }

    template <class T>
    T Vector2<T>::Length() const
    {
        return T(std::sqrt(LengthSqr()));
    }

    template <class T>
    Vector2<T> Vector2<T>::ProjectOnPlane(Vector2 aUp, Vector2 aVector)
    {
        auto y = aUp * aVector.y;
        auto x = Cross(aUp, 1) * aVector.x;
        return x + y;
    }

    template <class T>
    Vector2<T> Vector2<T>::GetNormalized() const
    {
        T len = Length();

        if(len == 0)
        {
            return { T(0), T(0) };
        }
        
        return *this * (1 / len);
    }

    template <class T>
    Vector2<T> Vector2<T>::Abs() const
    {
        return { abs(x), abs(y) };
    }

    template <class T>
    void Vector2<T>::Normalize()
    {
        *this = GetNormalized();
    }

    template <class T>
    Vector3<T> Vector2<T>::ToVec3(T aZ)
    {
        return { x, y, aZ };
    }

    template <class T>
    T Vector2<T>::Dot(const Vector2& aVector) const
    {
        return x * aVector.x + y * aVector.y;
    }

    //Returns the vector sum of aVector0 and aVector1
    template <class T>
    Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
    {
        return Vector2<T>((aVector0).x + (aVector1).x, (aVector0).y + (aVector1).y);
    }

    //Returns the vector difference of aVector0 and aVector1
    template <class T>
    Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
    {
        return Vector2<T>((aVector0).x - (aVector1).x, (aVector0).y - (aVector1).y);
    }

    //Returns the vector aVector1 component-multiplied by aVector1
    template <class T>
    Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T> aVector1)
    {
        return Vector2<T>((aVector0).x * (aVector1).x, (aVector0).y * (aVector1).y);
    }

    //Returns the vector aVector multiplied by the scalar aScalar
    template <class T>
    Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
    {
        return Vector2<T>((aVector).x * (aScalar), (aVector).y * (aScalar));
    }

    //Returns the vector aVector multiplied by the scalar aScalar
    template <class T>
    Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
    {
        return Vector2<T>((aVector).x * (aScalar), (aVector).y * (aScalar));
    }

    //Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
    template <class T>
    Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
    {
        return Vector2<T>((aVector).x / (aScalar), (aVector).y / (aScalar));
    }

    //Returns the vector aVector divided by the vector aVectorB
    template <class T>
    Vector2<T> operator/(const Vector2<T>& aVector, const Vector2<T>& aVectorB)
    {
        return Vector2<T>(aVector.x / aVectorB.x, aVector.y / aVectorB.y);
    }

    //Equivalent to setting aVector0 to (aVector0 + aVector1)
    template <class T>
    void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
    {
        aVector0 = aVector0 + aVector1;
    }

    //Equivalent to setting aVector0 to (aVector0 - aVector1)
    template <class T>
    void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
    {
        aVector0 = aVector0 - aVector1;
    }

    //Equivalent to setting aVector to (aVector * aScalar)
    template <class T>
    void operator*=(Vector2<T>& aVector, const T& aScalar)
    {
        aVector = aVector * aScalar;
    }

    //Equivalent to setting aVector to (aVector / aScalar)
    template <class T>
    void operator/=(Vector2<T>& aVector, const T& aScalar)
    {
        aVector = aVector / aScalar;
    }
}

namespace CU = CommonUtilities;