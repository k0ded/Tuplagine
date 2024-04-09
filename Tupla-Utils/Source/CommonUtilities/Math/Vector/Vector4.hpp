#pragma once
#include <complex>

namespace CommonUtilities
{
    template <class T>
    class Matrix4x4;
    
    template <class T = float>
    class Vector4
    {
    public:
        union 
        {
            struct 
            {
                T x;
                T y;
                T z;
                T w;
            };
            
            struct 
            {
                T r;
                T g;
                T b;
                T a;
            };

            T data[4];
        };
        
        //Creates a null-vector
        Vector4() = default;
        //Creates a vector (aX, aY, aZ)
        Vector4(const T& aX, const T& aY, const T& aZ, const T& aW);
        Vector4(const T aData[4]);
        //Copy constructor (compiler generated)
        Vector4(const Vector4& aVector);
        template <class OtherVectorClass>
        Vector4(const OtherVectorClass& aOther);

        //Destructor (compiler generated)
        ~Vector4();
        //Explicit Type operator, create a different vector with the same values.
        //Example creates a Tga::Vector4<T> from this CommonUtillities::Vector4<T>
        template <class OtherVectorClass>
        operator OtherVectorClass() const;
        //Assignment operator (compiler generated)
        Vector4& operator=(const Vector4& aVector4) = default;
        //Returns a negated copy of the vector
        Vector4 operator-() const;
        T& operator[](size_t aIndex);
        const T& operator[](size_t aIndex) const;
        //Returns the dot product of this and aVector
        T Dot(const Vector4& aVector) const;
        //Returns the squared length of the vector
        T LengthSqr() const;
        //Returns the length of the vector
        T Length() const;
        //Returns a normalized copy of this vector
        Vector4 GetNormalized() const;
        //Normalizes the vector
        void Normalize();
        
        Vector3<T> ToVec3() const;
        Vector2<T> ToVec2() const;
    };

    template <class T>
    Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
    {
        x = aX;
        y = aY;
        z = aZ;
        w = aW;
    }

    template <class T>
    Vector4<T>::Vector4(const T aData[4])
    {
        data = aData;
    }

    template <class T>
    Vector4<T>::Vector4(const Vector4& aVector)
    {
        x = aVector.x;
        y = aVector.y;
        z = aVector.z;
        w = aVector.w;
    }

    template <class T>
    template <class OtherVectorClass>
    Vector4<T>::Vector4(const OtherVectorClass& aOther) :
        x{ T(aOther.x) }, y{ T(aOther.y) }, z{ T(aOther.z) }, w{ T(aOther.w) }
    {
    }

    template <class T>
    Vector4<T>::~Vector4() = default;

    template <class T>
    template <class OtherVectorClass>
    Vector4<T>::operator OtherVectorClass() const
    {
        return { x, y, z, w };
    }

    template <class T>
    Vector4<T> Vector4<T>::operator-() const
    {
        return *this * T(-1);
    }

    template <class T>
    T& Vector4<T>::operator[](size_t aIndex)
    {
        assert(aIndex < 4 && "Subscript operator out of range");
        return data[aIndex];
    }

    template <class T>
    const T& Vector4<T>::operator[](size_t aIndex) const
    {
        assert(aIndex < 4 && "Subscript operator out of range");
        return data[aIndex];
    }

    template <class T>
    T Vector4<T>::LengthSqr() const
    {
        return x*x + y*y + z*z + w*w;
    }

    template <class T>
    T Vector4<T>::Length() const
    {
        return T(std::sqrt(LengthSqr()));
    }

    template <class T>
    Vector4<T> Vector4<T>::GetNormalized() const
    {
        T len = Length();

        if(len == 0)
        {
            return { T(0), T(0), T(0), T(0) };
        }
        
        return *this * (1 / len);
    }

    template <class T>
    void Vector4<T>::Normalize()
    {
        *this = GetNormalized();
    }

    template <class T>
    Vector3<T> Vector4<T>::ToVec3() const
    {
        return { x, y, w };
    }

    template <class T>
    Vector2<T> Vector4<T>::ToVec2() const
    {
        return { x, y }; 
    }

    template <class T>
    T Vector4<T>::Dot(const Vector4& aVector) const
    {
        return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
    }

    //Returns the vector sum of aVector0 and aVector1
    template <class T>
    Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
    {
        return Vector4<T>((aVector0).x + (aVector1).x, (aVector0).y + (aVector1).y, (aVector0).z + (aVector1).z, (aVector0.w) + (aVector1.w));
    }

    //Returns the vector difference of aVector0 and aVector1
    template <class T>
    Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
    {
        return Vector4<T>((aVector0).x - (aVector1).x, (aVector0).y - (aVector1).y, (aVector0).z - (aVector1).z, (aVector0.w) - (aVector1.w));
    }

    //Returns the vector aVector1 component-multiplied by aVector1
    template <class T>
    Vector4<T> operator*(const Vector4<T>& aVector0, const Vector4<T> aVector1)
    {
        return Vector4<T>((aVector0).x * (aVector1).x, (aVector0).y * (aVector1).y, (aVector0).z * (aVector1).z, (aVector0.w) * (aVector1.w));
    }

    //Returns the vector aVector multiplied by the scalar aScalar
    template <class T>
    Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
    {
        return Vector4<T>((aVector).x * (aScalar), (aVector).y * (aScalar), (aVector).z * (aScalar), (aVector.w) * (aScalar));
    }

    //Returns the vector aVector multiplied by the scalar aScalar
    template <class T>
    Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
    {
        return Vector4<T>((aVector).x * (aScalar), (aVector).y * (aScalar), (aVector).z * (aScalar), (aVector.w) * (aScalar));
    }

    //Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
    template <class T>
    Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
    {
        return Vector4<T>((aVector).x / (aScalar), (aVector).y / (aScalar), (aVector).z / (aScalar), (aVector.w) / (aScalar));
    }

    //Returns the vector aVector divided by the vector aVectorB
    template <class T>
    Vector4<T> operator/(const Vector4<T>& aVector, const Vector4<T>& aVectorB)
    {
        return Vector4<T>(aVector.x / aVectorB.x, aVector.y / aVectorB.y, aVector.z / aVectorB.z, aVector.w / aVectorB.w);
    }

    //Equivalent to setting aVector0 to (aVector0 + aVector1)
    template <class T>
    void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
    {
        aVector0 = aVector0 + aVector1;
    }

    //Equivalent to setting aVector0 to (aVector0 - aVector1)
    template <class T>
    void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
    {
        aVector0 = aVector0 - aVector1;
    }

    //Equivalent to setting aVector to (aVector * aScalar)
    template <class T>
    void operator*=(Vector4<T>& aVector, const T& aScalar)
    {
        aVector = aVector * aScalar;
    }

    //Equivalent to setting aVector to (aVector / aScalar)
    template <class T>
    void operator/=(Vector4<T>& aVector, const T& aScalar)
    {
        aVector = aVector / aScalar;
    }

    template <class T>
    bool operator==(const Vector4<T>& aVector, const Vector4<T>& aVector1)
    {
        return aVector.x == aVector1.x && aVector.y == aVector1.y && aVector.z == aVector1.z && aVector.w == aVector1.w;
    }

    typedef Vector4<unsigned>   Vector4ui;
    typedef Vector4<double>     Vector4d;
    typedef Vector4<float>      Vector4f;
    typedef Vector4<int>        Vector4i;
}

namespace CU = CommonUtilities;