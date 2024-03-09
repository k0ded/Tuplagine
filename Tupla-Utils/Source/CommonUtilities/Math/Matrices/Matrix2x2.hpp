#pragma once
#include <array>
#include <assert.h>

#include "../Vector/Vector2.hpp"

namespace CommonUtilities
{
    template <typename T>
    class Matrix2x2
    {
    public:
#pragma warning( disable :4201) // Nonstandard nameless struct/union.
        union
        {
            std::array<T, 4> LinearArray;
            std::array<std::array<T, 2>, 2> DoubleArray;
            std::array<Vector2<T>, 2> VectorArray;
        };
        
        // Creates the identity matrix.
        Matrix2x2();
        
        ~Matrix2x2();

        // Copy Constructor.
        Matrix2x2(const Matrix2x2& aMatrix);

        // Linear array to 4x4 matrix.
        explicit Matrix2x2(const std::array<T, 4>& aLinearArray);

        // Double array to 4x4 matrix.
        explicit Matrix2x2(const std::array<std::array<T, 2>, 2>& aDoubleArray);

        // () operator for accessing element (row, column) for read/write or read, respectively.
        T& operator()(int aRow, int aColumn);
        const T& operator()(int aRow, int aColumn) const;

        const T& operator[](size_t aIndex) const;
        T& operator[](size_t aIndex);
        
        Matrix2x2 operator+(const Matrix2x2& aOther) const;
        Matrix2x2 operator-(const Matrix2x2& aOther) const;
        Matrix2x2 operator*(const Matrix2x2& aOther) const;
        bool operator==(const Matrix2x2& aOther) const;
        
        Matrix2x2& operator+=(const Matrix2x2& aOther);
        Matrix2x2& operator-=(const Matrix2x2& aOther);
        Matrix2x2& operator*=(const Matrix2x2& aOther);
        Matrix2x2& operator=(const Matrix2x2& aOther);
        
        // Transform vectors
        [[nodiscard]] float GetRotation() const;

        // Creates a transposed copy of the matrix.
        Matrix2x2 GetTranspose() const;
        
        // Static functions for creating rotation matrices.
        static Matrix2x2 CreateRotation(T aAngleInRadians);
        
        Matrix2x2 Abs();
    };

    template <typename T>
    Vector2<T> operator*(const Vector2<T>& aVector, const Matrix2x2<T>& aMatrix)
    {
        Vector2<T> resultingVector(
            aVector.x * aMatrix.LinearArray[0] + aVector.y * aMatrix.LinearArray[2],
            aVector.x * aMatrix.LinearArray[1] + aVector.y * aMatrix.LinearArray[3]
        );
        return resultingVector;
    }

    template <typename T>
    Matrix2x2<T>::Matrix2x2()
    {
        DoubleArray[0] = {1, 0};
        DoubleArray[1] = {0, 1};
    }

    template <typename T>
    Matrix2x2<T>::~Matrix2x2() = default;

    template <typename T>
    Matrix2x2<T>::Matrix2x2(const Matrix2x2& aMatrix)
    {
        LinearArray = aMatrix.LinearArray;
    }

    template <typename T>
    Matrix2x2<T>::Matrix2x2(const std::array<T, 4>& aLinearArray)
    {
        LinearArray = aLinearArray;
    }

    template <typename T>
    Matrix2x2<T>::Matrix2x2(const std::array<std::array<T, 2>, 2>& aDoubleArray)
    {
        DoubleArray = aDoubleArray;
    }

    template <typename T>
    T& Matrix2x2<T>::operator()(const int aRow, const int aColumn)
    {
        assert(aRow <= DoubleArray.size() && aRow > 0 && "Row is out of bounds!");
        assert(aColumn <= DoubleArray.size() && aColumn > 0 && "Column is out of bounds!");
        return DoubleArray[aRow - 1][aColumn - 1];
    }

    template <typename T>
    const T& Matrix2x2<T>::operator()(const int aRow, const int aColumn) const
    {
        assert(aRow <= DoubleArray.size() && aRow > 0 && "Row is out of bounds!");
        assert(aColumn <= DoubleArray.size() && aColumn > 0 && "Column is out of bounds!");
        return DoubleArray[aRow - 1][aColumn - 1];
    }
    
    template <typename T>
    const T& Matrix2x2<T>::operator[](size_t aIndex) const
    {
        return LinearArray[aIndex];
    }

    template <typename T>
    T& Matrix2x2<T>::operator[](size_t aIndex)
    {
        return LinearArray[aIndex];
    }

    template <typename T>
    Matrix2x2<T> Matrix2x2<T>::operator+(const Matrix2x2& aOther) const
    {
        std::array<T, 4> result = {};
        for (int i = 0; i < LinearArray.size(); ++i)
        {
            result[i] = LinearArray[i] + aOther.LinearArray[i];
        }

        return Matrix2x2(result);
    }
    
    template <typename T>
    Matrix2x2<T> Matrix2x2<T>::operator-(const Matrix2x2& aOther) const
    {
        std::array<T, 4> result = {};
        for (int i = 0; i < LinearArray.size(); ++i)
        {
            result[i] = LinearArray[i] - aOther.LinearArray[i];
        }

        return Matrix2x2(result);
    }

    template <typename T>
    Matrix2x2<T> Matrix2x2<T>::operator*(const Matrix2x2& aOther) const
    {
        std::array<std::array<T, 2>, 2> result = {};

        for (int x = 0; x < 2; ++x)
        {
            for (int y = 0; y < 2; ++y)
            {
                const T r1 = DoubleArray[x][0] * aOther.DoubleArray[0][y];
                const T r2 = DoubleArray[x][1] * aOther.DoubleArray[1][y];
                result[x][y] = r1 + r2;
            }
        }

        return Matrix2x2(result);
    }

    template <typename T>
    bool Matrix2x2<T>::operator==(const Matrix2x2& aOther) const
    {
        return this == &aOther || LinearArray == aOther.LinearArray;
    }

    template <typename T>
    Matrix2x2<T>& Matrix2x2<T>::operator+=(const Matrix2x2& aOther)
    {
        auto result = *this + aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix2x2<T>& Matrix2x2<T>::operator-=(const Matrix2x2& aOther)
    {
        auto result = *this - aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix2x2<T>& Matrix2x2<T>::operator*=(const Matrix2x2& aOther)
    {
        auto result = *this * aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix2x2<T>& Matrix2x2<T>::operator=(const Matrix2x2& aOther)
    {
        if (this == &aOther)
            return *this;

        LinearArray = aOther.LinearArray;
        return *this;
    }

    /**
     * \brief Gets the euler angles of a rotation in radians.
     * \return Rotations in each axis in radians!
     */
    template <typename T>
    float Matrix2x2<T>::GetRotation() const
    {
        return atan2(DoubleArray[0][1], DoubleArray[0][0]);
    }

    template <typename T>
    Matrix2x2<T> Matrix2x2<T>::GetTranspose() const
    {
        return Matrix2x2(
            std::array<T, 4> {
                DoubleArray[0][0], DoubleArray[1][0],
                DoubleArray[0][1], DoubleArray[1][1]
            }
        );
    }

    template <typename T>
    Matrix2x2<T> Matrix2x2<T>::CreateRotation(T aAngleInRadians)
    {
        const T c = std::cos(aAngleInRadians);
        const T s = std::sin(aAngleInRadians);
        return Matrix2x2(
            std::array<T, 4> {
                 c, s, 
                -s, c
            }
        );
    }

    template <typename T>
    Matrix2x2<T> Matrix2x2<T>::Abs()
    {
        return Matrix2x2(std::array<T, 4> {
                abs(LinearArray[0]), abs(LinearArray[1]),
                abs(LinearArray[2]), abs(LinearArray[3])
            }
        );
    }
}
