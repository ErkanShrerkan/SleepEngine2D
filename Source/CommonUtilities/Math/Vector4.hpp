#pragma once
#include "Vector3.hpp"
namespace CommonUtilities
{
	//struct W
	//{
	//	union
	//	{

	//	};
	//};


	template <class T>
	class Vector4
	{
	public:
		#pragma warning(disable:4201)
		union
		{
			struct
			{
				union
				{
					Vector2<T> xy;
					Vector2<T> rg;
				};
				union
				{
					Vector2<T> zw;
					Vector2<T> ba;
				};
			};
			struct
			{
				union
				{
					T x;
					// Alias for x
					T r, red, _1;
				};
				struct
				{
					union
					{
						Vector3<T> yzw;
						Vector3<T> gba;
					};
				};
			};
			struct
			{
				union
				{
					struct
					{
						union
						{
							T x;
							// Alias for x
							T r, red, _1;
						};
						union
						{
							T y;
							// Alias for y
							T g, green, _2;
						};
						union
						{
							T z;
							// Alias for z
							T b, blue, _3;
						};
					};
					union
					{
						Vector3<T> xyz;
						Vector3<T> rgb;
					};
				};
				union
				{
					T w;
					// Alias for w
					T a, alpha, _4;
				};
			};
		};
		#pragma warning(default:4201)
		

		//Creates a null-vector
		Vector4<T>()
		{
			x = T();
			y = T();
			z = T();
			w = T();
		}

		//Creates a vector (aX, aY, aZ, aW)
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW)
		{
			x = aX;
			y = aY;
			z = aZ;
			w = aW;
		}

		Vector4<T>(const Vector3<T>& aXYZ, const T& aW)
		{
			x = aXYZ._1;
			y = aXYZ._2;
			z = aXYZ._3;
			w = aW;
		}

		Vector4<T>(const T& aX, const Vector3<T>& aYZW)
		{
			x = aX;
			y = aYZW._1;
			z = aYZW._2;
			w = aYZW._3;
		}

		Vector4<T>(const Vector2<T>& aXY, const Vector2<T>& aZW)
		{
			x = aXY._1;
			y = aXY._2;
			z = aZW._1;
			w = aZW._2;
		}

		Vector4<T>(const Vector2<T>& aXY, const T& aZ, const T& aW)
		{
			x = aXY._1;
			y = aXY._2;
			z = aZ;
			w = aW;
		}

		Vector4<T>(const T& aX, const Vector2<T>& aYZ, const T& aW)
		{
			x = aX;
			y = aYZ._1;
			z = aYZ._2;
			w = aW;
		}

		Vector4<T>(const T& aX, const T& aY, const Vector2<T>& aZW)
		{
			x = aX;
			y = aY;
			z = aZW._1;
			w = aZW._2;
		}

		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;

		//Destructor (compiler generated)
		~Vector4<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return x * x + y * y + z * z + w * w;
		}

		//Returns the length of the vector
		T Length() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		//Returns a normalized copy of this
		Vector4<T> GetNormalized() const
		{
			if (x == T() && y == T() && z == T() && w == T())
			{
				return Vector4();
			}

			Vector4<T> temp(x, y, z, w);
			T length = Length();
			temp.x /= length;
			temp.y /= length;
			temp.z /= length;
			temp.w /= length;
			return temp;
		}

		//Normalizes the vector
		void Normalize()
		{
			*this = (*this).GetNormalized();
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const
		{
			T temp = x * aVector.x;
			temp += y * aVector.y;
			temp += z * aVector.z;
			temp += w * aVector.w;
			return temp;
		}

		//A Vector with value of 1 in each axis
		static constexpr Vector4<T> One()
		{
			return Vector4<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));
		}

		template <class U>
		Vector4<U> AsVector()
		{
			return Vector4<U>(
				static_cast<U>(x),
				static_cast<U>(y),
				static_cast<U>(z),
				static_cast<U>(w)
			);
		}

		// Sets all components (x, y, z, w) to aValue
		void SetAll(const T& aValue)
		{
			x = aValue;
			y = aValue;
			z = aValue;
			w = aValue;
		}
	};

	//Returns the vector sum of aVector0 and aVector1
	template<class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp = aVector0;
		temp.x += aVector1.x;
		temp.y += aVector1.y;
		temp.z += aVector1.z;
		temp.w += aVector1.w;
		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template<class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp = aVector0;
		temp.x -= aVector1.x;
		temp.y -= aVector1.y;
		temp.z -= aVector1.z;
		temp.w -= aVector1.w;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> temp = aVector;
		temp.x *= aScalar;
		temp.y *= aScalar;
		temp.z *= aScalar;
		temp.w *= aScalar;
		return temp;
	}

	template<class T> Vector4<T> operator*(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> temp = aVector0;
		temp.x *= aVector1.x;
		temp.y *= aVector1.y;
		temp.z *= aVector1.z;
		temp.w *= aVector1.w;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> temp = aVector;
		temp.x *= aScalar;
		temp.y *= aScalar;
		temp.z *= aScalar;
		temp.w *= aScalar;
		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template<class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> temp = aVector;
		temp.x /= aScalar;
		temp.y /= aScalar;
		temp.z /= aScalar;
		temp.w /= aScalar;
		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template<class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template<class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}

	//Equivalentto setting aVector to (aVector * aScalar)
	template<class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template<class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}
}