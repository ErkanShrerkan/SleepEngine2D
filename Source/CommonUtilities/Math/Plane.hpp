#pragma once
#include "Vector.hpp"
#include "Matrix4x4.hpp"

namespace CommonUtilities
{
	template <class T>
	class Plane
	{
	public:
		T a;
		T b;
		T c;
		T d;

		Vector3<T> myNormal;
		Vector3<T> myPoint1;
		Vector3<T> myPoint2;
		Vector3<T> myPoint3;

		Plane()
		{
			myNormal = Vector3<T>(0, 0, 0);
			myPoint1 = Vector3<T>(0, 0, 0);
			myPoint2 = Vector3<T>(0, 0, 0);
			myPoint3 = Vector3<T>(0, 0, 0);
			a = 0;
			b = 0;
			c = 0;
			d = 0;
		}

		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
		{
			InitWith3Points(aPoint0, aPoint1, aPoint2);
		}

		Plane(const Vector3<T>& aPoint0, const Vector3<T>& aNormal)
		{
			InitWithPointAndNormal(aPoint0, aNormal);
		}

		Plane Transform(const Matrix4x4<T>& aTransform)
		{
			Plane p;
			p.myPoint1 = Vector4<T>(Vector4<T>(myPoint1, 1) * aTransform).xyz;
			p.myPoint2 = Vector4<T>(Vector4<T>(myPoint2, 1) * aTransform).xyz;
			p.myPoint3 = Vector4<T>(Vector4<T>(myPoint3, 1) * aTransform).xyz;
			p.myNormal = Vector4<T>(Vector4<T>(myNormal, 0) * aTransform).xyz;
			p.a = p.myNormal.x;
			p.b = p.myNormal.y;
			p.c = p.myNormal.z;
			p.d = -p.myPoint1.Dot(p.myNormal);
			return p;
		}

		void InitWith3Points(const Vector3<T>& aPoint0, const Vector3<T>& aPoint1, const Vector3<T>& aPoint2)
		{
			myPoint1 = aPoint0;
			myPoint2 = aPoint1;
			myPoint3 = aPoint2;
			myNormal = Vector3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
			myNormal.Normalize();
			a = myNormal.x;
			b = myNormal.y;
			c = myNormal.z;
			d = -aPoint0.Dot(myNormal);
			myNormal.Normalize();
		}

		void InitWithPointAndNormal(const Vector3<T>& aPoint, const Vector3<T>& aNormal)
		{
			myPoint1 = aPoint;
			myNormal = aNormal.GetNormalized();
			a = myNormal.x;
			b = myNormal.y;
			c = myNormal.z;
			d = -aPoint.Dot(myNormal);
		}

		void FlipNormal()
		{
			myNormal *= -1.0f;
			a = myNormal.x;
			b = myNormal.y;
			c = myNormal.z;
			d = -myPoint1.Dot(myNormal);
		}

		void SetABCD(T anA, T aB, T aC, T aD, Vector3<T>& aPoint)
		{
			a = anA;
			b = aB;
			c = aC;
			d = aD;

			myPoint1 = aPoint;

			myNormal = { a, b, c };
		}

		bool IsInside(const Vector3<T>& aPosition) const
		{
			return (myNormal.Dot(aPosition - myPoint1) <= 0);
		}

		const Vector3<T>& GetNormal() const
		{
			return myNormal;
		}
	};
}