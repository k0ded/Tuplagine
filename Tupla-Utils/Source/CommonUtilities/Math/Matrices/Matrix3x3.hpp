#pragma once
#include <array>
#include <assert.h>

#include "../Vector/Vector3.hpp"

namespace CommonUtilities
{
    template<typename T>
    class Matrix4x4;
    
    template <typename T>
    class Matrix3x3
    {
    public:
        union
        {
            std::array<T, 9> LinearArray;
            std::array<std::array<T, 3>, 3> DoubleArray;
        };

        // Creates the identity matrix.
        Matrix3x3();

        ~Matrix3x3();

        // Copy Constructor.
        Matrix3x3(const Matrix3x3& aMatrix);

        // Copies the top left 3x3 part of the Matrix4x4.
        Matrix3x3(const Matrix4x4<T>& aMatrix);

        // Linear array to 3x3 matrix.
        explicit Matrix3x3(const std::array<T, 9>& aLinearArray);

        // Double array to 3x3 matrix.
        explicit Matrix3x3(const std::array<std::array<T, 3>, 3>& aDoubleArray);

        // () operator for accessing element (row, column) for read/write or read, respectively.
        T& operator()(int aRow, int aColumn);
        const T& operator()(int aRow, int aColumn) const;

        const T& operator[](size_t aIndex) const;
        T& operator[](size_t aIndex);

        Matrix3x3 operator+(const Matrix3x3& aOther) const;
        Matrix3x3 operator-(const Matrix3x3& aOther) const;
        Matrix3x3 operator*(const Matrix3x3& aOther) const;
        bool operator==(const Matrix3x3& aOther) const;

        Matrix3x3& operator+=(const Matrix3x3& aOther);
        Matrix3x3& operator-=(const Matrix3x3& aOther);
        Matrix3x3& operator*=(const Matrix3x3& aOther);
        Matrix3x3& operator=(const Matrix3x3& aOther);

        // Creates a transposed copy of the matrix.
        Matrix3x3 GetTranspose() const;

        // Assumes aTransform is made up of nothing but rotations and translations.
        // Is detailed in the second matrix lecture
        Matrix3x3 GetFastInverse() const;

        // Static functions for creating rotation matrices.
        static Matrix3x3 CreateRotationAroundX(T aAngleInRadians);
        static Matrix3x3 CreateRotationAroundY(T aAngleInRadians);
        static Matrix3x3 CreateRotationAroundZ(T aAngleInRadians);
    };

    template <typename T>
    Vector3<T> operator*(const Vector3<T> aVector3, const Matrix3x3<T>& aMatrix)
    {
        Vector3<T> resultingVector(
            aVector3.x * aMatrix.LinearArray[0] + aVector3.y * aMatrix.LinearArray[3] + aVector3.z * aMatrix.LinearArray[6],
            aVector3.x * aMatrix.LinearArray[1] + aVector3.y * aMatrix.LinearArray[4] + aVector3.z * aMatrix.LinearArray[7],
            aVector3.x * aMatrix.LinearArray[2] + aVector3.y * aMatrix.LinearArray[5] + aVector3.z * aMatrix.LinearArray[8]
        );
        return resultingVector;
    }

    template <typename T>
    Matrix3x3<T>::Matrix3x3()
    {
        DoubleArray[0] = {1, 0, 0};
        DoubleArray[1] = {0, 1, 0};
        DoubleArray[2] = {0, 0, 1};
    }

    template <typename T>
    Matrix3x3<T>::~Matrix3x3() = default;

    template <typename T>
    Matrix3x3<T>::Matrix3x3(const Matrix3x3& aMatrix)
    {
        LinearArray = aMatrix.LinearArray;
    }

    template <typename T>
    Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
    {
        DoubleArray[0] = { aMatrix(1, 1), aMatrix(1, 2), aMatrix(1, 3) };
        DoubleArray[1] = { aMatrix(2, 1), aMatrix(2, 2), aMatrix(2, 3) };
        DoubleArray[2] = { aMatrix(3, 1), aMatrix(3, 2), aMatrix(3, 3) };
    }

    template <typename T>
    Matrix3x3<T>::Matrix3x3(const std::array<T, 9>& aLinearArray)
    {
        LinearArray = aLinearArray;
    }

    template <typename T>
    Matrix3x3<T>::Matrix3x3(const std::array<std::array<T, 3>, 3>& aDoubleArray)
    {
        DoubleArray = aDoubleArray;
    }

    template <typename T>
    T& Matrix3x3<T>::operator()(int aRow, int aColumn)
    {
        assert(aRow <= 3 && aRow > 0 && "Row is out of bounds!");
        assert(aColumn <= 3 && aColumn > 0 && "Row is out of bounds!");
        return DoubleArray[aRow - 1][aColumn - 1];
    }

    template <typename T>
    const T& Matrix3x3<T>::operator()(int aRow, int aColumn) const
    {
        assert(aRow <= 3 && aRow > 0 && "Row is out of bounds!");
        assert(aColumn <= 3 && aColumn > 0 && "Row is out of bounds!");
        return DoubleArray[aRow - 1][aColumn - 1];
    }

    template <typename T>
    const T& Matrix3x3<T>::operator[](size_t aIndex) const
    {
        return LinearArray[aIndex];
    }

    template <typename T>
    T& Matrix3x3<T>::operator[](size_t aIndex)
    {
        return LinearArray[aIndex];
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3& aOther) const
    {
        std::array<T, 9> result = {};
        for (int i = 0; i < LinearArray.size(); ++i)
        {
            result[i] = LinearArray[i] + aOther.LinearArray[i];
        }

        return Matrix3x3(result);
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3& aOther) const
    {
        std::array<T, 9> result = {};
        for (int i = 0; i < LinearArray.size(); ++i)
        {
            result[i] = LinearArray[i] - aOther.LinearArray[i];
        }

        return Matrix3x3(result);
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3& aOther) const
    {
        std::array<std::array<T, 3>, 3> result = {};

        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                const T r1 = DoubleArray[x][0] * aOther.DoubleArray[0][y];
                const T r2 = DoubleArray[x][1] * aOther.DoubleArray[1][y];
                const T r3 = DoubleArray[x][2] * aOther.DoubleArray[2][y];
                result[x][y] = r1 + r2 + r3; 
            }
        }

        return Matrix3x3(result);
    }

    template <typename T>
    Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3& aOther)
    {
        Matrix3x3 result = *this + aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3& aOther)
    {
        Matrix3x3 result = *this - aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3& aOther)
    {
        Matrix3x3 result = *this * aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3& aOther)
    {
        LinearArray = aOther.LinearArray;
        return *this;
    }

    template <typename T>
    bool Matrix3x3<T>::operator==(const Matrix3x3& aOther) const
    {
        return this == &aOther || LinearArray == aOther.LinearArray;
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::GetTranspose() const
    {
        return Matrix3x3(
            std::array<T, 9> {
                DoubleArray[0][0], DoubleArray[1][0], DoubleArray[2][0],
                DoubleArray[0][1], DoubleArray[1][1], DoubleArray[2][1],
                DoubleArray[0][2], DoubleArray[1][2], DoubleArray[2][2]
            }
        );
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::GetFastInverse() const
    {
        Matrix3x3 rotationInversed = GetTranspose();
        Matrix3x3 translation;

        // Copy over an inverse of the translation from the rotation inverse
        translation[6] = -rotationInversed[2];
        translation[7] = -rotationInversed[5];

        // Get rid of the translation in the rotation inverse.
        rotationInversed[2] = 0;
        rotationInversed[5] = 0;
        
        return rotationInversed * translation;
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
    {
        return Matrix3x3(
            std::array<T, 9> {
                1, 0, 0,
                0, std::cos(aAngleInRadians), std::sin(aAngleInRadians),
                0, -std::sin(aAngleInRadians), std::cos(aAngleInRadians)
            }
        );
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
    {
        return Matrix3x3(
            std::array<T, 9> {
                std::cos(aAngleInRadians), 0, -std::sin(aAngleInRadians),
                0, 1, 0,
                std::sin(aAngleInRadians), 0, std::cos(aAngleInRadians)
            }
        );
    }

    template <typename T>
    Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
    {
        return Matrix3x3(
            std::array<T, 9> {
                std::cos(aAngleInRadians), std::sin(aAngleInRadians), 0,
                -std::sin(aAngleInRadians), std::cos(aAngleInRadians), 0,
                0, 0, 1
            }
        );
    }
}
