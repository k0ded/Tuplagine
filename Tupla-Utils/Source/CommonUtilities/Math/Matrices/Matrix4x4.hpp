#pragma once
#include <array>
#include <assert.h>
#include <immintrin.h>
#include <valarray>

#include "../Vector/Vector3.hpp"
#include "../Vector/Vector4.hpp"
#include "CommonUtilities/Math/Math.hpp"

namespace CommonUtilities
{
    template <typename T>
    class Matrix4x4
    {
    public:
#pragma warning( disable :4201) // Nonstandard nameless struct/union.
        union
        {
            std::array<T, 16> LinearArray;
            std::array<std::array<T, 4>, 4> DoubleArray;
            std::array<Vector4<T>, 4> VectorArray;
            struct 
            {
                __m128 m1;
                __m128 m2;
                __m128 m3;
                __m128 m4;
            };
        };
        
        // Creates the identity matrix.
        Matrix4x4();
        
        ~Matrix4x4();

        // Copy Constructor.
        Matrix4x4(const Matrix4x4& aMatrix);

        // Linear array to 4x4 matrix.
        explicit Matrix4x4(const std::array<T, 16>& aLinearArray);

        // Double array to 4x4 matrix.
        explicit Matrix4x4(const std::array<std::array<T, 4>, 4>& aDoubleArray);

        // () operator for accessing element (row, column) for read/write or read, respectively.
        T& operator()(int aRow, int aColumn);
        const T& operator()(int aRow, int aColumn) const;

        const Vector4<T>& operator[](size_t aIndex) const;
        Vector4<T>& operator[](size_t aIndex);
        
        Matrix4x4 operator+(const Matrix4x4& aOther) const;
        Matrix4x4 operator-(const Matrix4x4& aOther) const;
        Matrix4x4 operator*(const Matrix4x4& aOther) const;
        bool operator==(const Matrix4x4& aOther) const;
        
        Matrix4x4& operator+=(const Matrix4x4& aOther);
        Matrix4x4& operator-=(const Matrix4x4& aOther);
        Matrix4x4& operator*=(const Matrix4x4& aOther);
        Matrix4x4& operator=(const Matrix4x4& aOther);

        // Direction vectors
        Vector3<T> GetForward() const;
        Vector3<T> GetUp() const;
        Vector3<T> GetRight() const;
        
        // Transform vectors
        Vector3<T> GetPosition() const;
        Vector3<T> GetRotation() const;
        Vector3<T> GetScale() const;

        // Returns whether or not the decomposition was successful
        bool DecomposeTransform(Vector3<T>& aTranslation, Vector3<T>& aRotation, Vector3<T>& aScale) const;
        
        void SetPosition(Vector3<T> aPosition);
        void SetRotation(Vector3<T> aRotationInRads);
        void SetScale(Vector3<T> aScale);

        // Creates a transposed copy of the matrix.
        Matrix4x4 GetTranspose() const;
        
        // Assumes aTransform is made up of nothing but rotations and translations.
        // Is detailed in the second matrix lecture
        Matrix4x4 GetFastInverse() const;
        
        // Static functions for creating rotation matrices.
        static Matrix4x4 CreateRotationAroundX(T aAngleInRadians);
        static Matrix4x4 CreateRotationAroundY(T aAngleInRadians);
        static Matrix4x4 CreateRotationAroundZ(T aAngleInRadians);
        static Matrix4x4 CreateRotation(Vector3<T> aAngleInRadians);
        static Matrix4x4<T> CreatePerspectiveProjection(float aAspect, float aHorizontalFov, float aFar, float aNear);
    };

    template <typename T>
    Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
    {
        Vector4<T> resultingVector(
            aVector.x * aMatrix.LinearArray[0] + aVector.y * aMatrix.LinearArray[4] + aVector.z * aMatrix.LinearArray[8] + aVector.w * aMatrix.LinearArray[12],
            aVector.x * aMatrix.LinearArray[1] + aVector.y * aMatrix.LinearArray[5] + aVector.z * aMatrix.LinearArray[9] + aVector.w * aMatrix.LinearArray[13],
            aVector.x * aMatrix.LinearArray[2] + aVector.y * aMatrix.LinearArray[6] + aVector.z * aMatrix.LinearArray[10] + aVector.w * aMatrix.LinearArray[14],
            aVector.x * aMatrix.LinearArray[3] + aVector.y * aMatrix.LinearArray[7] + aVector.z * aMatrix.LinearArray[11] + aVector.w * aMatrix.LinearArray[15]
        );
        return resultingVector;
    }

    template <>
    inline Vector4<float> operator*(const Vector4<float>& aVector, const Matrix4x4<float>& aMatrix)
    {
        Vector4<float> aVec;
        const __m128 x = _mm_set1_ps(aVector.x);
        const __m128 y = _mm_set1_ps(aVector.y);
        const __m128 z = _mm_set1_ps(aVector.z);
        const __m128 w = _mm_set1_ps(aVector.w);
        const __m128 result = _mm_add_ps(
            _mm_add_ps(
            _mm_mul_ps(x, aMatrix.m1),
            _mm_mul_ps(y, aMatrix.m2)
        ),
        _mm_add_ps(
            _mm_mul_ps(z, aMatrix.m3),
            _mm_mul_ps(w, aMatrix.m4)
            )
        );

        _mm_store_ps(reinterpret_cast<float*>(&aVec), result);
        return aVec;
    }

    template <typename T>
    Vector3<T> MultiplyPoint(const Vector3<T>& aVector, const Matrix4x4<T>& aMatrix)
    {
        Vector4<T> resultingVector(
            aVector.x * aMatrix.LinearArray[0] + aVector.y * aMatrix.LinearArray[4] + aVector.z * aMatrix.LinearArray[8] + aMatrix.LinearArray[12],
            aVector.x * aMatrix.LinearArray[1] + aVector.y * aMatrix.LinearArray[5] + aVector.z * aMatrix.LinearArray[9] + aMatrix.LinearArray[13],
            aVector.x * aMatrix.LinearArray[2] + aVector.y * aMatrix.LinearArray[6] + aVector.z * aMatrix.LinearArray[10] + aMatrix.LinearArray[14],
            aVector.x * aMatrix.LinearArray[3] + aVector.y * aMatrix.LinearArray[7] + aVector.z * aMatrix.LinearArray[11] + aMatrix.LinearArray[15]
        );
        return resultingVector;
    }

    template <>
    inline Vector3<float> MultiplyPoint(const Vector3<float>& aVector, const Matrix4x4<float>& aMatrix)
    {
        Vector4<float> vec;
        const __m128 x = _mm_set1_ps(aVector.x);
        const __m128 y = _mm_set1_ps(aVector.y);
        const __m128 z = _mm_set1_ps(aVector.z);
        const __m128 result = _mm_add_ps(
            _mm_add_ps(
            _mm_mul_ps(x, aMatrix.m1),
            _mm_mul_ps(y, aMatrix.m2)
        ),
        _mm_add_ps(
            _mm_mul_ps(z, aMatrix.m3),
            aMatrix.m4
            )
        );

        _mm_store_ps(reinterpret_cast<float*>(&vec), result);
        return vec.ToVec3();
    }

    template <typename T>
    Vector3<T> MultiplyVector(const Vector3<T>& aVector, const Matrix4x4<T>& aMatrix)
    {
        Vector4<T> resultingVector(
            aVector.x * aMatrix.LinearArray[0] + aVector.y * aMatrix.LinearArray[4] + aVector.z * aMatrix.LinearArray[8],
            aVector.x * aMatrix.LinearArray[1] + aVector.y * aMatrix.LinearArray[5] + aVector.z * aMatrix.LinearArray[9],
            aVector.x * aMatrix.LinearArray[2] + aVector.y * aMatrix.LinearArray[6] + aVector.z * aMatrix.LinearArray[10],
            aVector.x * aMatrix.LinearArray[3] + aVector.y * aMatrix.LinearArray[7] + aVector.z * aMatrix.LinearArray[11]
        );
        return resultingVector;
    }

    template <>
    inline Vector3<float> MultiplyVector(const Vector3<float>& aVector, const Matrix4x4<float>& aMatrix)
    {
        Vector4<float> vec;
        const __m128 x = _mm_set1_ps(aVector.x);
        const __m128 y = _mm_set1_ps(aVector.y);
        const __m128 z = _mm_set1_ps(aVector.z);
        const __m128 result = _mm_add_ps(
            _mm_add_ps(
            _mm_mul_ps(x, aMatrix.m1),
            _mm_mul_ps(y, aMatrix.m2)
            ),
            _mm_mul_ps(z, aMatrix.m3)
        );

        _mm_store_ps(reinterpret_cast<float*>(&vec), result);
        return vec.ToVec3();
    }

    template <typename T>
    Matrix4x4<T>::Matrix4x4()
    {
        DoubleArray[0] = {1, 0, 0, 0};
        DoubleArray[1] = {0, 1, 0, 0};
        DoubleArray[2] = {0, 0, 1, 0};
        DoubleArray[3] = {0, 0, 0, 1};
    }

    template <typename T>
    Matrix4x4<T>::~Matrix4x4() = default;

    template <typename T>
    Matrix4x4<T>::Matrix4x4(const Matrix4x4& aMatrix)
    {
        LinearArray = aMatrix.LinearArray;
    }

    template <typename T>
    Matrix4x4<T>::Matrix4x4(const std::array<T, 16>& aLinearArray)
    {
        LinearArray = aLinearArray;
    }

    template <typename T>
    Matrix4x4<T>::Matrix4x4(const std::array<std::array<T, 4>, 4>& aDoubleArray)
    {
        DoubleArray = aDoubleArray;
    }

    template <typename T>
    T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
    {
        assert(aRow <= DoubleArray.size() && aRow > 0 && "Row is out of bounds!");
        assert(aColumn <= DoubleArray.size() && aColumn > 0 && "Column is out of bounds!");
        return DoubleArray[aRow - 1][aColumn - 1];
    }

    template <typename T>
    const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
    {
        assert(aRow <= DoubleArray.size() && aRow > 0 && "Row is out of bounds!");
        assert(aColumn <= DoubleArray.size() && aColumn > 0 && "Column is out of bounds!");
        return DoubleArray[aRow - 1][aColumn - 1];
    }
    
    template <typename T>
    const Vector4<T>& Matrix4x4<T>::operator[](size_t aIndex) const
    {
        return VectorArray[aIndex];
    }

    template <typename T>
    Vector4<T>& Matrix4x4<T>::operator[](size_t aIndex)
    {
        return VectorArray[aIndex];
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4& aOther) const
    {
        std::array<T, 16> result = {};
        for (int i = 0; i < LinearArray.size(); ++i)
        {
            result[i] = LinearArray[i] + aOther.LinearArray[i];
        }

        return Matrix4x4(result);
    }
    
    template <>
    inline Matrix4x4<double> Matrix4x4<double>::operator+(const Matrix4x4& aOther) const
    {
        std::array<double, 16> result = {};

        for (int i = 0; i < 4; ++i)
        {
            const __m256d row = _mm256_load_pd(&LinearArray[4 * i]);
            const __m256d otherRow = _mm256_load_pd(&aOther.LinearArray[4 * i]);
            _mm256_store_pd(&result[4 * i], _mm256_add_pd(row, otherRow));
        }

        return Matrix4x4(result);
    }

    template <>
    inline Matrix4x4<float> Matrix4x4<float>::operator+(const Matrix4x4& aOther) const
    {
        std::array<float, 16> result = {};

        for (int i = 0; i < 2; ++i)
        {
            const __m256 row = _mm256_load_ps(&LinearArray[8 * i]);
            const __m256 otherRow = _mm256_load_ps(&aOther.LinearArray[8 * i]);
            _mm256_store_ps(&result[8 * i], _mm256_add_ps(row, otherRow));
        }

        return Matrix4x4(result);
    }
    
    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4& aOther) const
    {
        std::array<T, 16> result = {};
        for (int i = 0; i < LinearArray.size(); ++i)
        {
            result[i] = LinearArray[i] - aOther.LinearArray[i];
        }

        return Matrix4x4(result);
    }

    template <>
    inline Matrix4x4<double> Matrix4x4<double>::operator-(const Matrix4x4& aOther) const
    {
        std::array<double, 16> result = {};

        for (int i = 0; i < 4; ++i)
        {
            const __m256d row = _mm256_load_pd(&LinearArray[4 * i]);
            const __m256d otherRow = _mm256_load_pd(&aOther.LinearArray[4 * i]);
            _mm256_store_pd(&result[4 * i], _mm256_sub_pd(row, otherRow));
        }

        return Matrix4x4(result);
    }
    
    template <>
    inline Matrix4x4<float> Matrix4x4<float>::operator-(const Matrix4x4& aOther) const
    {
        std::array<float, 16> result = {};

        for (int i = 0; i < 2; ++i)
        {
            const __m256 row = _mm256_load_ps(&LinearArray[8 * i]);
            const __m256 otherRow = _mm256_load_ps(&aOther.LinearArray[8 * i]);
            _mm256_store_ps(&result[8 * i], _mm256_sub_ps(row, otherRow));
        }

        return Matrix4x4(result);
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4& aOther) const
    {
        std::array<std::array<T, 4>, 4> result = {};

        for (int x = 0; x < 4; ++x)
        {
            for (int y = 0; y < 4; ++y)
            {
                const T r1 = DoubleArray[x][0] * aOther.DoubleArray[0][y];
                const T r2 = DoubleArray[x][1] * aOther.DoubleArray[1][y];
                const T r3 = DoubleArray[x][2] * aOther.DoubleArray[2][y];
                const T r4 = DoubleArray[x][3] * aOther.DoubleArray[3][y];
                result[x][y] = r1 + r2 + r3 + r4; 
            }
        }

        return Matrix4x4(result);
    }

    template <>
    inline Matrix4x4<double> Matrix4x4<double>::operator*(const Matrix4x4& aOther) const
    {
        std::array<double, 16> result = {};
        const __m256d row1 = _mm256_load_pd(&aOther.LinearArray[0]);
        const __m256d row2 = _mm256_load_pd(&aOther.LinearArray[4]);
        const __m256d row3 = _mm256_load_pd(&aOther.LinearArray[8]);
        const __m256d row4 = _mm256_load_pd(&aOther.LinearArray[12]);
        for(int i=0; i<4; i++) {
            const __m256d brod1 = _mm256_set1_pd(LinearArray[4*i + 0]);
            const __m256d brod2 = _mm256_set1_pd(LinearArray[4*i + 1]);
            const __m256d brod3 = _mm256_set1_pd(LinearArray[4*i + 2]);
            const __m256d brod4 = _mm256_set1_pd(LinearArray[4*i + 3]);
            const __m256d row = _mm256_add_pd(
                        _mm256_add_pd(
                            _mm256_mul_pd(brod1, row1),
                            _mm256_mul_pd(brod2, row2)),
                        _mm256_add_pd(
                            _mm256_mul_pd(brod3, row3),
                            _mm256_mul_pd(brod4, row4)));
            _mm256_store_pd(&result[4*i], row);
        }

        return Matrix4x4(result);
    }

    template <>
    inline Matrix4x4<float> Matrix4x4<float>::operator*(const Matrix4x4& aOther) const
    {
        std::array<float, 16> result = {};
        const __m128 row1 = _mm_load_ps(&aOther.LinearArray[0]);
        const __m128 row2 = _mm_load_ps(&aOther.LinearArray[4]);
        const __m128 row3 = _mm_load_ps(&aOther.LinearArray[8]);
        const __m128 row4 = _mm_load_ps(&aOther.LinearArray[12]);
        for(int i=0; i<4; i++) {
            const __m128 brod1 = _mm_set1_ps(LinearArray[4*i + 0]);
            const __m128 brod2 = _mm_set1_ps(LinearArray[4*i + 1]);
            const __m128 brod3 = _mm_set1_ps(LinearArray[4*i + 2]);
            const __m128 brod4 = _mm_set1_ps(LinearArray[4*i + 3]);
            const __m128 row = _mm_add_ps(
                        _mm_add_ps(
                            _mm_mul_ps(brod1, row1),
                            _mm_mul_ps(brod2, row2)),
                        _mm_add_ps(
                            _mm_mul_ps(brod3, row3),
                            _mm_mul_ps(brod4, row4)));
            _mm_store_ps(&result[4*i], row);
        }

        return Matrix4x4(result);
    }

    template <typename T>
    bool Matrix4x4<T>::operator==(const Matrix4x4& aOther) const
    {
        return this == &aOther || LinearArray == aOther.LinearArray;
    }

    template <typename T>
    Matrix4x4<T>& Matrix4x4<T>::operator+=(const Matrix4x4& aOther)
    {
        auto result = *this + aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4& aOther)
    {
        auto result = *this - aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4& aOther)
    {
        auto result = *this * aOther;
        LinearArray = result.LinearArray;
        return *this;
    }

    template <typename T>
    Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4& aOther)
    {
        if (this == &aOther)
            return *this;

        LinearArray = aOther.LinearArray;
        return *this;
    }

    /**
     * \brief Not normalized!
     * \return The forward vector part of the matrix
     */
    template <typename T>
    Vector3<T> Matrix4x4<T>::GetForward() const
    {
        return { DoubleArray[2][0], DoubleArray[2][1], DoubleArray[2][2] };
    }

    /**
     * \brief Not normalized!
     * \return The up vector part of the matrix
     */
    template <typename T>
    Vector3<T> Matrix4x4<T>::GetUp() const
    {
        return { DoubleArray[1][0], DoubleArray[1][1], DoubleArray[1][2] };
    }

    /**
     * \brief Not normalized!
     * \return The right vector part of the matrix
     */
    template <typename T>
    Vector3<T> Matrix4x4<T>::GetRight() const
    {
        return { DoubleArray[0][0], DoubleArray[0][1], DoubleArray[0][2] };
    }

    template <typename T>
    Vector3<T> Matrix4x4<T>::GetScale() const
    {
        return { GetRight().Length(), GetUp().Length(), GetForward().Length() };
    }

    template <typename T>
    bool Matrix4x4<T>::DecomposeTransform(Vector3<T>& aTranslation, Vector3<T>& aRotation, Vector3<T>& aScale) const
    {
        using namespace Nerd;
        Matrix4x4 localMatrix = *this;

        // Normalize the matrix.
        if (Compare(localMatrix[3][3], static_cast<T>(0), Epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (
            Compare(localMatrix[0][3], static_cast<T>(0), Epsilon<T>()) ||
            Compare(localMatrix[1][3], static_cast<T>(0), Epsilon<T>()) ||
            Compare(localMatrix[2][3], static_cast<T>(0), Epsilon<T>()))
        {
            // Clear the perspective partition
            localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
            localMatrix[3][3] = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        aTranslation = localMatrix[3].ToVec3();
        localMatrix[3] = Vector4<T>(0, 0, 0, localMatrix[3].w);

        Vector3<T> Row[3]/*, Pdum3 */;

        // Now get scale and shear.
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < 3; ++j)
                Row[i][j] = localMatrix[i][j];

        // Compute X scale factor and normalize first row.
        aScale.x = length(Row[0]);
        Row[0].Normalize();
        aScale.y = length(Row[1]);
        Row[1].Normalize();
        aScale.z = length(Row[2]);
        Row[2].Normalize();

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
    #if 0
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);
                Row[i] *= static_cast<T>(-1);
            }
        }
    #endif

        aRotation.y = asin(-Row[0][2]);
        if (cos(aRotation.y) != 0) {
            aRotation.x = atan2(Row[1][2], Row[2][2]);
            aRotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else {
            aRotation.x = atan2(-Row[2][0], Row[1][1]);
            aRotation.z = 0;
        }


        return true;
    }

    template <typename T>
    void Matrix4x4<T>::SetPosition(Vector3<T> aPosition)
    {
        VectorArray[3] = aPosition.ToVec4(1);
    }

    template <typename T>
    void Matrix4x4<T>::SetRotation(Vector3<T> aRotationInRads)
    {
        Matrix4x4 matrix = CreateRotationAroundZ(aRotationInRads.z) * CreateRotationAroundY(aRotationInRads.y) * CreateRotationAroundX(aRotationInRads.x);
        Vector3<T> scale = GetScale();
        VectorArray[0] = matrix.VectorArray[0] * scale.x;
        VectorArray[1] = matrix.VectorArray[1] * scale.y;
        VectorArray[2] = matrix.VectorArray[2] * scale.z;
    }

    template <typename T>
    void Matrix4x4<T>::SetScale(Vector3<T> aScale)
    {
        VectorArray[0].Normalize();
        VectorArray[1].Normalize();
        VectorArray[2].Normalize();
        
        VectorArray[0] *= aScale.x;
        VectorArray[1] *= aScale.y;
        VectorArray[2] *= aScale.z;
    }

    template <typename T>
    Vector3<T> Matrix4x4<T>::GetPosition() const
    {
        return { DoubleArray[3][0], DoubleArray[3][1], DoubleArray[3][2] };
    }

    /**
     * \brief Gets the euler angles of a rotation in radians.
     * \return Rotations in each axis in radians!
     */
    template <typename T>
    Vector3<T> Matrix4x4<T>::GetRotation() const
    {
        Vector3<T> rotation;
        rotation.y = asin(-DoubleArray[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(DoubleArray[1][2], DoubleArray[2][2]);
            rotation.z = atan2(DoubleArray[0][1], DoubleArray[0][0]);
        }
        else {
            rotation.x = atan2(-DoubleArray[2][0], DoubleArray[1][1]);
            rotation.z = 0;
        }
        
        return rotation;
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::GetTranspose() const
    {
        return Matrix4x4(
            std::array<T, 16>{
                DoubleArray[0][0], DoubleArray[1][0], DoubleArray[2][0], DoubleArray[3][0],
                DoubleArray[0][1], DoubleArray[1][1], DoubleArray[2][1], DoubleArray[3][1],
                DoubleArray[0][2], DoubleArray[1][2], DoubleArray[2][2], DoubleArray[3][2],
                DoubleArray[0][3], DoubleArray[1][3], DoubleArray[2][3], DoubleArray[3][3],
            }
        );
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::GetFastInverse() const
    {
        Matrix4x4 rotationInversed = GetTranspose();
        Matrix4x4 translation;

        // Copy over an inverse of the translation from the rotation inverse
        translation[12] = -rotationInversed[3];
        translation[13] = -rotationInversed[7];
        translation[14] = -rotationInversed[11];

        // Get rid of the translation in the rotation inverse.
        rotationInversed[3] = 0;
        rotationInversed[7] = 0;
        rotationInversed[11] = 0;
        
        return translation * rotationInversed;
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
    {
        return Matrix4x4(
            std::array<T, 16>{
                1, 0, 0, 0,
                0, std::cos(aAngleInRadians), std::sin(aAngleInRadians), 0,
                0, -std::sin(aAngleInRadians), std::cos(aAngleInRadians), 0,
                0, 0, 0, 1
            }
        );
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
    {
        return Matrix4x4(
            std::array<T, 16>{
                std::cos(aAngleInRadians), 0, -std::sin(aAngleInRadians), 0,
                0, 1, 0, 0,
                std::sin(aAngleInRadians), 0, std::cos(aAngleInRadians), 0,
                0, 0, 0, 1
            }
        );
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
    {
        return Matrix4x4(
            std::array<T, 16>{
                std::cos(aAngleInRadians), std::sin(aAngleInRadians), 0, 0,
                -std::sin(aAngleInRadians), std::cos(aAngleInRadians), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            }
        );
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::CreateRotation(Vector3<T> aAngleInRadians)
    {
        return CreateRotationAroundZ(aAngleInRadians.z) * CreateRotationAroundY(aAngleInRadians.y) * CreateRotationAroundX(aAngleInRadians.x);
    }

    template <typename T>
    Matrix4x4<T> Matrix4x4<T>::CreatePerspectiveProjection(float aAspect, float aHorizontalFov, float aFar, float aNear)
    {
        const float vFov = 2 * atan(tan(aHorizontalFov * 0.5f) * aAspect);
	    const float t = tan(vFov * 0.5f);

        return Matrix4x4(
            std::array<T, 16> {
				aAspect * 1.0f / t, 0                 , 0                              , 0,
                0                 , 1.0f / t          , 0                              , 0,
                0                 , 0                 ,aFar / (aFar - aNear)           , 1,
                0                 , 0                 ,-aNear * (aFar / (aFar - aNear)), 0
			}
        );
    }
}
