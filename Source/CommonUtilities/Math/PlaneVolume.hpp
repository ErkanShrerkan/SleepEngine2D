#pragma once
#include <vector>
#include "Vector.hpp"
#include "Plane.hpp"

namespace CommonUtilities
{
	template <class T>
	class PlaneVolume
	{
	public:
		PlaneVolume()
		{

		}

		PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
		{
			myPlanes = aPlaneList;
		}

		void AddPlane(const Plane<T>& aPlane)
		{
			myPlanes.push_back(aPlane);
		}

		void Clear()
		{
			myPlanes.clear();
		}

		void Resize(unsigned aSize)
		{
			myPlanes.resize(aSize);
		}

		PlaneVolume Transform(const Matrix4x4<T>& aTransform)
		{
			PlaneVolume pv;
			for (auto& plane : myPlanes)
			{
				pv.AddPlane(plane.Transform(aTransform));
			}
			return pv;
		}

		std::vector<Plane<T>>& GetPlanes()
		{
			return myPlanes;
		}

		const std::vector<Plane<T>>& GetPlanes() const 
		{
			return myPlanes;
		}

		bool IsInside(const Vector3<T>& aPosition)
		{
			for (Plane<T>& plane : myPlanes)
			{
				if (!plane.IsInside(aPosition))
				{
					return false;
				}
			}

			return true;
		}

	private:
		std::vector<Plane<T>> myPlanes;
	};
}