#pragma once
#include "Vector4.hpp"
#include "Vector3.hpp"
#include <cmath>
#include <intrin.h>

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4
	{
	public:
		// Creates the identity matrix.
		Matrix4x4<T>();

		// Copy constructor
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix) = default;

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int& aRow, const int& aColumn);
		const T& operator()(const int& aRow, const int& aColumn) const;
		const T& operator[](const int anIndex) const;
		T& operator[](const int anIndex);

		// Static functions for creating rotation matrices.
		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundVector(const Vector3<T>& aVector, T anAngle);

		// Static function for creating a transpose of a matrix.
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);

		// Static function to get a hacked version of inverse. Works only on rotation with translation matrices
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aMatrixToFastInverse);
		Matrix4x4<T> FastInverse();
		Matrix4x4<T>& Normalize();

		static const Matrix4x4<T> IdentityMatrix;

		void SetRow(int aOneBasedIndex, Vector4<T> aVector4);
		void SetColumn(int aOneBasedIndex, Vector4<T> aVector4);

		// Extra
		const Vector3<T> GetTranslation() const;
		void SetTranslation(const Vector3<T>& aTranslation);
		const Vector3<T> GetRotation() const;
		void SetRotation(const Matrix4x4<T>& anOtherMatrix);
		void Rotate(const Vector3<T>& aRotation);

		// reinterpret_cast Getters
		Vector4<T>& GetRow(int aOneBasedindex) const;
		Vector3<T>& GetRight();
		Vector3<T>& GetUp();
		Vector3<T>& GetForward();
		Vector3<T>& GetPosition();
		const Vector3<T>& GetRight() const;
		const Vector3<T>& GetUp() const;
		const Vector3<T>& GetForward() const;
		const Vector3<T>& GetPosition() const;


		// Operator Overloading
		Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix);
		const Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix);
		Matrix4x4<T> operator*(Matrix4x4<T>& aMatrix);
		const Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix);
		const bool operator==(const Matrix4x4<T>& aMatrix) const;
		void operator+=(const Matrix4x4<T>& aMatrix);
		void operator-=(const Matrix4x4<T>& aMatrix);
		void operator*=(const Matrix4x4<T>& aMatrix);
		Vector4<T> operator*(const Vector4<T>& aVector);

		Matrix4x4<T> operator*(const T aScalar);
		void operator*=(const T aScalar);

		T* const Raw();

		void Clear()
		{
			memset(this, 0, sizeof(Matrix4x4<T>));
		}

	private:
		union
		{
			T myMatrix[4][4];
			T mySingle[16];
		};
	};
	template<class T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		for (unsigned int row = 0; row < 4; row++)
		{
			for (unsigned int col = 0; col < 4; col++)
			{
				if (row == col)
				{
					myMatrix[row][col] = static_cast<T>(1);
				}
				else
				{
					myMatrix[row][col] = T();
				}
			}
		}
	}
	template<class T>
	inline T& Matrix4x4<T>::operator()(const int& aRow, const int& aColumn)
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline const T& Matrix4x4<T>::operator()(const int& aRow, const int& aColumn) const
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline const T& Matrix4x4<T>::operator[](const int anIndex) const
	{
		return mySingle[anIndex];
	}
	template<class T>
	inline T& Matrix4x4<T>::operator[](const int anIndex)
	{
		return mySingle[anIndex];
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4 matrix;

		matrix(1, 1) = static_cast<T>(1);
		matrix(1, 2) = static_cast<T>(0);
		matrix(1, 3) = static_cast<T>(0);

		matrix(2, 1) = static_cast<T>(0);
		matrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
		matrix(2, 3) = static_cast<T>(sin(aAngleInRadians));

		matrix(3, 1) = static_cast<T>(0);
		matrix(3, 2) = static_cast<T>(-sin(aAngleInRadians));
		matrix(3, 3) = static_cast<T>(cos(aAngleInRadians));

		return matrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4 matrix;

		matrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
		matrix(1, 2) = static_cast<T>(0);
		matrix(1, 3) = static_cast<T>(-sin(aAngleInRadians));

		matrix(2, 1) = static_cast<T>(0);
		matrix(2, 2) = static_cast<T>(1);
		matrix(2, 3) = static_cast<T>(0);

		matrix(3, 1) = static_cast<T>(sin(aAngleInRadians));
		matrix(3, 2) = static_cast<T>(0);
		matrix(3, 3) = static_cast<T>(cos(aAngleInRadians));

		return matrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4 matrix;

		matrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
		matrix(1, 2) = static_cast<T>(sin(aAngleInRadians));
		matrix(1, 3) = static_cast<T>(0);

		matrix(2, 1) = static_cast<T>(-sin(aAngleInRadians));
		matrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
		matrix(2, 3) = static_cast<T>(0);

		matrix(3, 1) = static_cast<T>(0);
		matrix(3, 2) = static_cast<T>(0);
		matrix(3, 3) = static_cast<T>(1);

		return matrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundVector(const Vector3<T>& aVector, T anAngle)
	{
		Vector3<T> normalizedVector = aVector.GetNormalized();

		// rodrigues
		Matrix4x4<T> rotation;
		rotation(1, 1) = 0;
		rotation(1, 2) = -normalizedVector.z;
		rotation(1, 3) = normalizedVector.y;

		rotation(2, 1) = normalizedVector.z;
		rotation(2, 2) = 0;
		rotation(2, 3) = -normalizedVector.x;

		rotation(3, 1) = -normalizedVector.y;
		rotation(3, 2) = normalizedVector.x;
		rotation(3, 3) = 0;

		Matrix4x4<T> identity;

		// Causing error since T * Matrix<T> is not defined
		//rotation = identity + (rotation * std::sin(anAngle)) + (1 - std::cos(anAngle)) * (rotation * rotation);
		// Fixed by swapping to Matrix<T> * T
		rotation = (identity + (rotation * std::sin(anAngle))) + (rotation * rotation) * (1 - std::cos(anAngle));

		rotation(4, 4) = 1;

		return rotation;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4 matrix;
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int col = 1; col < 5; col++)
			{
				matrix(row, col) = aMatrixToTranspose(col, row);
			}
		}
		return matrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aMatrixToFastInverse)
	{
		// Inversen av R
		Matrix4x4 matrixR = Matrix4x4::Transpose(aMatrixToFastInverse);
		matrixR(1, 4) = T();
		matrixR(2, 4) = T();
		matrixR(3, 4) = T();

		// Inversen av T
		Matrix4x4 matrixT;
		matrixT(4, 1) = -aMatrixToFastInverse(4, 1);
		matrixT(4, 2) = -aMatrixToFastInverse(4, 2);
		matrixT(4, 3) = -aMatrixToFastInverse(4, 3);

		// Inversen av RT
		return matrixT * matrixR;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::FastInverse()
	{
		return Matrix4x4<T>::GetFastInverse(*this);
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::Normalize()
	{
		GetRow(1).Normalize();
		GetRow(2).Normalize();
		GetRow(3).Normalize();
		return *this;
	}

	template<class T>
	inline void Matrix4x4<T>::SetRow(int aOneBasedIndex, Vector4<T> aVector4)
	{
		(*this)(aOneBasedIndex, 1) = aVector4.x;
		(*this)(aOneBasedIndex, 2) = aVector4.y;
		(*this)(aOneBasedIndex, 3) = aVector4.z;
		(*this)(aOneBasedIndex, 4) = aVector4.w;
	}
	template<class T>
	inline void Matrix4x4<T>::SetColumn(int aOneBasedIndex, Vector4<T> aVector4)
	{
		(*this)(1, aOneBasedIndex) = aVector4.x;
		(*this)(2, aOneBasedIndex) = aVector4.y;
		(*this)(3, aOneBasedIndex) = aVector4.z;
		(*this)(4, aOneBasedIndex) = aVector4.w;
	}
	template<class T>
	inline const Vector3<T> Matrix4x4<T>::GetTranslation() const
	{
		return Vector3<T>(
			(*this)(4, 1),
			(*this)(4, 2),
			(*this)(4, 3)
			);
	}
	template<class T>
	inline void Matrix4x4<T>::SetTranslation(const Vector3<T>& aTranslation)
	{
		(*this)(4, 1) = aTranslation.x;
		(*this)(4, 2) = aTranslation.y;
		(*this)(4, 3) = aTranslation.z;
	}
	template<class T>
	inline const Vector3<T> Matrix4x4<T>::GetRotation() const
	{
		return Vector3<T>(
			(*this)(1, 1),
			(*this)(2, 2),
			(*this)(3, 3)
			);
	}
	template<class T>
	inline void Matrix4x4<T>::SetRotation(const Matrix4x4<T>& anOtherMatrix)
	{
		for (int row = 1; row <= 3; row++)
		{
			for (int col = 1; col <= 3; col++)
			{
				(*this)(row, col) = anOtherMatrix(row, col);
			}
		}
	}
	constexpr const float pi = 3.14159265358979323851280895940618620443274267017841339111328125f;
	constexpr const float radian = pi / 180.f;
	template<class T>
	inline void Matrix4x4<T>::Rotate(const Vector3<T>& aRotation)
	{
		Matrix4x4<T> t;
		t *= Matrix4x4<T>::CreateRotationAroundX(aRotation.x * radian);
		t *= Matrix4x4<T>::CreateRotationAroundY(aRotation.y * radian);
		t *= Matrix4x4<T>::CreateRotationAroundZ(aRotation.z * radian);
		Vector4<T> pos = { GetTranslation(), 1.f };
		t.SetRow(4, pos);
		*this = t;
	}
	template<class T>
	inline Vector4<T>& Matrix4x4<T>::GetRow(int aOneBasedindex) const
	{
		return *reinterpret_cast<Vector4<T>*>(const_cast<T*>(&myMatrix[aOneBasedindex - 1][0]));
	}
	template<class T>
	inline Vector3<T>& Matrix4x4<T>::GetRight()
	{
		return GetRow(1).xyz;
	}
	template<class T>
	inline Vector3<T>& Matrix4x4<T>::GetUp()
	{
		return GetRow(2).xyz;
	}
	template<class T>
	inline Vector3<T>& Matrix4x4<T>::GetForward()
	{
		return GetRow(3).xyz;
	}
	template<class T>
	inline Vector3<T>& Matrix4x4<T>::GetPosition()
	{
		return GetRow(4).xyz;
	}
	template<class T>
	inline const Vector3<T>& Matrix4x4<T>::GetRight() const
	{
		return GetRow(1).xyz;
	}
	template<class T>
	inline const Vector3<T>& Matrix4x4<T>::GetUp() const
	{
		return GetRow(2).xyz;
	}
	template<class T>
	inline const Vector3<T>& Matrix4x4<T>::GetForward() const
	{
		return GetRow(3).xyz;
	}
	template<class T>
	inline const Vector3<T>& Matrix4x4<T>::GetPosition() const
	{
		return GetRow(4).xyz;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4 matrix(*this);
		matrix += aMatrix;
		return matrix;
	}
	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4 matrix(*this);
		matrix -= aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4 matrix(*this);
		matrix *= aMatrix;
		return matrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(Matrix4x4<T>& aMatrix)
	{
		Matrix4x4 matrix(*this);
		matrix *= aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int col = 1; col < 5; col++)
			{
				(*this)(row, col) = aMatrix(row, col);
			}
		}
		return *this;
	}
	template<class T>
	inline const bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aMatrix) const
	{
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int col = 1; col < 5; col++)
			{
				if ((*this)(row, col) != aMatrix(row, col))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<class T>
	inline void Matrix4x4<T>::operator+=(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int col = 1; col < 5; col++)
			{
				(*this)(row, col) += aMatrix(row, col);
			}
		}
	}
	template<class T>
	inline void Matrix4x4<T>::operator-=(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int col = 1; col < 5; col++)
			{
				(*this)(row, col) -= aMatrix(row, col);
			}
		}
	}
	template<class T>
	inline void Matrix4x4<T>::operator*=(const Matrix4x4<T>& aMatrix)
	{
		// Matrismultiplikation
		Matrix4x4<T> matrix;

		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int col = 1; col < 5; col++)
			{
				matrix(row, col) = 0;

				for (unsigned int i = 1; i < 5; i++)
				{
					matrix(row, col) += (*this)(row, i) * aMatrix(i, col);
				}
			}
		}

		(*this) = matrix;
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::operator*(const Vector4<T>& aVector)
	{
		Vector4<T> newVector;

		newVector.x = (aVector.x * (*this)(1, 1) + aVector.y * (*this)(2, 1) + aVector.z * (*this)(3, 1) + aVector.w * (*this)(4, 1));
		newVector.y = (aVector.x * (*this)(1, 2) + aVector.y * (*this)(2, 2) + aVector.z * (*this)(3, 2) + aVector.w * (*this)(4, 2));
		newVector.z = (aVector.x * (*this)(1, 3) + aVector.y * (*this)(2, 3) + aVector.z * (*this)(3, 3) + aVector.w * (*this)(4, 3));
		newVector.w = (aVector.x * (*this)(1, 4) + aVector.y * (*this)(2, 4) + aVector.z * (*this)(3, 4) + aVector.w * (*this)(4, 4));

		return newVector;
	}

	template<>
	inline void Matrix4x4<float>::operator*=(const Matrix4x4<float>& aFloatMatrix)
	{
		// When the Matrix multiplication is of type float,
		// we can optimize the multiplication using SIMD

		__m128 BCx = _mm_load_ps(&aFloatMatrix.myMatrix[0][0]);
		const __m128 BCy = _mm_load_ps(&aFloatMatrix.myMatrix[1][0]);
		const __m128 BCz = _mm_load_ps(&aFloatMatrix.myMatrix[2][0]);
		const __m128 BCw = _mm_load_ps(&aFloatMatrix.myMatrix[3][0]);

		Matrix4x4 result;
		float* leftRowPointer = &mySingle[0];
		float* resultRowPointer = &result.mySingle[0];

		for (unsigned i = 0; i < 4; ++i)
		{
			__m128 ARx = _mm_set_ps1(leftRowPointer[0]);
			__m128 ARy = _mm_set_ps1(leftRowPointer[1]);
			__m128 ARz = _mm_set_ps1(leftRowPointer[2]);
			__m128 ARw = _mm_set_ps1(leftRowPointer[3]);

			__m128 x = _mm_mul_ps(ARx, BCx);
			__m128 y = _mm_mul_ps(ARy, BCy);
			__m128 z = _mm_mul_ps(ARz, BCz);
			__m128 w = _mm_mul_ps(ARw, BCw);

			__m128 xy = _mm_add_ps(x, y);
			__m128 zw = _mm_add_ps(z, w);
			__m128 xyzw = _mm_add_ps(xy, zw);
			_mm_storeu_ps(resultRowPointer, xyzw);

			leftRowPointer += 4;
			resultRowPointer += 4;
		}

		*this = result;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const T aScalar)
	{
		Matrix4x4 copy(*this);
		copy *= aScalar;
		return copy;
	}

	template<class T>
	inline void Matrix4x4<T>::operator*=(const T aScalar)
	{
		for (unsigned int row = 1; row <= 4; ++row)
		{
			for (unsigned int column = 1; column <= 4; ++column)
			{
				(*this)(row, column) *= aScalar;
			}
		}
	}

	template<class T>
	inline T* const Matrix4x4<T>::Raw()
	{
		return &mySingle[0];
	}

	// Vector4 * Matrix4x4 overloading
	template<class T>
	Vector4<T> operator*(const Vector4<T>& aLhsVector4, const Matrix4x4<T>& aRhsMatrix)
	{
		Vector4<T> vector;

		vector.x =
			aLhsVector4.x * aRhsMatrix(1, 1)
			+ aLhsVector4.y * aRhsMatrix(2, 1)
			+ aLhsVector4.z * aRhsMatrix(3, 1)
			+ aLhsVector4.w * aRhsMatrix(4, 1);
		vector.y =
			aLhsVector4.x * aRhsMatrix(1, 2)
			+ aLhsVector4.y * aRhsMatrix(2, 2)
			+ aLhsVector4.z * aRhsMatrix(3, 2)
			+ aLhsVector4.w * aRhsMatrix(4, 2);
		vector.z =
			aLhsVector4.x * aRhsMatrix(1, 3)
			+ aLhsVector4.y * aRhsMatrix(2, 3)
			+ aLhsVector4.z * aRhsMatrix(3, 3)
			+ aLhsVector4.w * aRhsMatrix(4, 3);
		vector.w =
			aLhsVector4.x * aRhsMatrix(1, 4)
			+ aLhsVector4.y * aRhsMatrix(2, 4)
			+ aLhsVector4.z * aRhsMatrix(3, 4)
			+ aLhsVector4.w * aRhsMatrix(4, 4);

		return vector;
	}
}