#pragma once
#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Matrix4x4.hpp"
namespace CommonUtilities
{
	template <class T>
	class Matrix2x2
	{
	public:

		// Creates the identity matrix.
		Matrix2x2<T>();

		Matrix2x2<T>(CommonUtilities::Vector4<T> aVector);

		// Copy constructor
		Matrix2x2<T>(const Matrix2x2<T>&) = default;

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// Static functions for creating rotation matrices.
		static Matrix2x2<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix2x2<T> CreateRotationAroundY(T aAngleInRadians);

		// Operator Overloading
		const Matrix2x2<T> operator+(const Matrix2x2<T>& aMatrix);
		const Matrix2x2<T> operator-(const Matrix2x2<T>& aMatrix);
		const Matrix2x2<T> operator*(const Matrix2x2<T>& aMatrix);
		const Matrix2x2<T>& operator=(const Matrix2x2<T>& aMatrix);
		const bool operator==(const Matrix2x2<T>& aMatrix) const;
		void operator+=(const Matrix2x2<T>& aMatrix);
		void operator-=(const Matrix2x2<T>& aMatrix);
		void operator*=(const Matrix2x2<T>& aMatrix);

	private:

		T myMatrix[2][2];
	};
	template<class T>
	inline Matrix2x2<T>::Matrix2x2()
	{
		for (unsigned int row = 0; row < 2; row++)
		{
			for (unsigned int col = 0; col < 2; col++)
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
	inline Matrix2x2<T>::Matrix2x2(CommonUtilities::Vector4<T> aVector)
	{
	}
	template<class T>
	inline T& Matrix2x2<T>::operator()(const int aRow, const int aColumn)
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline const T& Matrix2x2<T>::operator()(const int aRow, const int aColumn) const
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline Matrix2x2<T> Matrix2x2<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix2x2 matrix;

		matrix(1, 1) = 1;
		matrix(1, 2) = 0;
		matrix(1, 3) = 0;

		matrix(2, 1) = 0;
		matrix(2, 2) = cos(aAngleInRadians);
		matrix(2, 3) = sin(aAngleInRadians);

		matrix(3, 1) = 0;
		matrix(3, 2) = -sin(aAngleInRadians);
		matrix(3, 3) = cos(aAngleInRadians);

		return matrix;
	}
	template<class T>
	inline Matrix2x2<T> Matrix2x2<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix2x2 matrix;

		matrix(1, 1) = cos(aAngleInRadians);
		matrix(1, 2) = 0;
		matrix(1, 3) = -sin(aAngleInRadians);

		matrix(2, 1) = 0;
		matrix(2, 2) = 1;
		matrix(2, 3) = 0;

		matrix(3, 1) = sin(aAngleInRadians);
		matrix(3, 2) = 0;
		matrix(3, 3) = cos(aAngleInRadians);

		return matrix;
	}
	
	template<class T>
	inline const Matrix2x2<T> Matrix2x2<T>::operator+(const Matrix2x2<T>& aMatrix)
	{
		Matrix2x2 matrix(*this);
		matrix += aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix2x2<T> Matrix2x2<T>::operator-(const Matrix2x2<T>& aMatrix)
	{
		Matrix2x2 matrix(*this);
		matrix -= aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix2x2<T> Matrix2x2<T>::operator*(const Matrix2x2<T>& aMatrix)
	{
		Matrix2x2 matrix(*this);
		matrix *= aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix2x2<T>& Matrix2x2<T>::operator=(const Matrix2x2<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				(*this)(row, col) = aMatrix(row, col);
			}
		}
		return *this;
	}
	template<class T>
	inline const bool Matrix2x2<T>::operator==(const Matrix2x2<T>& aMatrix) const
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
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
	inline void Matrix2x2<T>::operator+=(const Matrix2x2<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				(*this)(row, col) += aMatrix(row, col);
			}
		}
	}
	template<class T>
	inline void Matrix2x2<T>::operator-=(const Matrix2x2<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				(*this)(row, col) -= aMatrix(row, col);
			}
		}
	}
	template<class T>
	inline void Matrix2x2<T>::operator*=(const Matrix2x2<T>& aMatrix)
	{
		// Matrismultiplikation
		Matrix2x2<T> matrix;

		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				matrix(row, col) = 0;

				for (unsigned int i = 1; i < 4; i++)
				{
					matrix(row, col) += (*this)(row, i) * aMatrix(i, col);
				}
			}
		}

		(*this) = matrix;
	}

	// Vector2 * Matrix2x2 overloading
	template<class T>
	Vector2<T> operator*(const Vector2<T>& aLhsVector2, const Matrix2x2<T>& aRhsMatrix)
	{
		Vector2<T> vector;

		vector.x =
			aLhsVector2.x * aRhsMatrix(1, 1)
			+ aLhsVector2.y * aRhsMatrix(2, 1);
		vector.y =
			aLhsVector2.x * aRhsMatrix(1, 2)
			+ aLhsVector2.y * aRhsMatrix(2, 2);

		return vector;
	}
}