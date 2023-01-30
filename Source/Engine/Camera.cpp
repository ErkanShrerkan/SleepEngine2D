#include "pch.h"
#include "Camera.h"
namespace SE
{
	CCamera::CCamera()
	{

	}

	CCamera::~CCamera()
	{
		
	}

	bool CCamera::Init(float aHorizontalFoV, const Vector2f& anAspectRatio, float aNear, float aFar)
	{
		myFOV = Math::Clamp(aHorizontalFoV, 1.f, 179.f);
		myRes = anAspectRatio;

		// Convert Degree to Radian
		float horizontalFoVinRadians = Math::DegreeToRadian(aHorizontalFoV);

		// Ideal vertical FoV based on aspect ratio
		float verticalFoVinRadians = 2.f * std::atanf(std::tanf(horizontalFoVinRadians * 0.5f) * (anAspectRatio.height / anAspectRatio.width));

		float myXScale = 1.f / std::tanf(horizontalFoVinRadians * 0.5f);
		float myYScale = 1.f / std::tanf(verticalFoVinRadians * 0.5f);

		myFarPlane = aFar; // TODO: Make "global"
		myNearPlane = aNear; // TODO: Make "global"

		float planeConstant = myFarPlane / (myFarPlane - myNearPlane);

		myProjection(1, 1) = myXScale;
		myProjection(2, 2) = myYScale;
		myProjection(3, 3) = planeConstant;
		myProjection(3, 4) = 1.f /*/ planeConstant*/;
		myProjection(4, 3) = (-myFarPlane * myNearPlane) / (myFarPlane - myNearPlane);
		myProjection(4, 4) = 0.0f;
		
		aHorizontalFoV = 180 - aHorizontalFoV;
		horizontalFoVinRadians = Math::DegreeToRadian(aHorizontalFoV);
		myXScale = 1.f / std::tanf(horizontalFoVinRadians * 0.5f);
		//aFar *= 0.5f;
		float3 nearCenter = { 0, 0, aNear };
		float3 farCenter = { 0, 0, aFar };
		float nearWidth = 2 * aNear * myXScale;
		float nearHeight = nearWidth * (anAspectRatio.height / anAspectRatio.width);
		float farWidth = 2 * aFar * myXScale;
		float farHeight = farWidth * (anAspectRatio.height / anAspectRatio.width);

		//nearWidth = 2 * aNear * myXScale;
		//nearHeight = nearWidth / ((float(PPBD_resolution.x) / float(PPBD_resolution.y)));

		//float3 cameraPos = FBD_cameraPosition.xyz;

		//float3 nearTopLeft =
		//	(forward * nearPlane) -
		//	(right * nearWidth * .5) +
		//	(up * nearHeight * .5);

		//dir = nearTopLeft + (right * uv.x * nearWidth) - (up * uv.y * nearHeight);

		myFrustum.Resize(6);
		auto& planes = myFrustum.GetPlanes();
		planes[0].InitWithPointAndNormal(nearCenter, { 0, 0, -1 });
		planes[1].InitWithPointAndNormal(farCenter, { 0, 0, 1 });

		float3 ntl = nearCenter + float3(0.f, nearHeight * .5f, 0) - float3(nearWidth * .5f, 0.f, 0.f);
		float3 ntr = ntl + float3(nearWidth, 0.f, 0.f);
		float3 nbl = ntl - float3(0, nearHeight, 0);
		float3 nbr = ntr - float3(0, nearHeight, 0);
		float3 ftl = farCenter + float3(0, farHeight * .5f, 0) - float3(farWidth * .5f, 0, 0);
		float3 ftr = ftl + float3(farWidth, 0, 0);
		float3 fbl = ftl - float3(0, farHeight, 0);
		float3 fbr = ftr - float3(0, farHeight, 0);

		planes[2].InitWith3Points(ntl, nbl, fbl);
		planes[3].InitWith3Points(ftr, fbr, nbr);
		planes[4].InitWith3Points(ntr, ntl, ftl);
		planes[5].InitWith3Points(fbl, nbl, nbr);

		return true;
	}

	bool CCamera::Init(float2 aResolution)
	{
		myFOV = 90;
		myRes = aResolution;

		myFarPlane = 100000.f;
		myNearPlane = 100.0f;

		myProjection(1, 1) = 2.f / aResolution.x;
		myProjection(2, 2) = 2.f / aResolution.y;
		myProjection(3, 3) = 1.f / (myFarPlane - myNearPlane);
		myProjection(4, 3) = myNearPlane / (myNearPlane - myFarPlane);
		myProjection(4, 4) = 1.0f;

		float3 nearCenter = { 0, 0, 10 };
		float3 farCenter = { 0, 0, 10000 };
		float width = aResolution.x;
		float height = aResolution.y;

		myFrustum.Resize(6);
		auto& planes = myFrustum.GetPlanes();
		planes[0].InitWithPointAndNormal(nearCenter, { 0, 0, -1 });
		planes[1].InitWithPointAndNormal(farCenter, { 0, 0, 1 });

		float3 ntl = nearCenter + float3(0.f, height * .5f, 0) - float3(width * .5f, 0.f, 0.f);
		float3 ntr = ntl + float3(width, 0.f, 0.f);
		float3 nbl = ntl - float3(0, height, 0);
		float3 nbr = ntr - float3(0, height, 0);
		float3 ftl = farCenter + float3(0, height * .5f, 0) - float3(width * .5f, 0, 0);
		float3 ftr = ftl + float3(width, 0, 0);
		float3 fbl = ftl - float3(0, height, 0);
		float3 fbr = ftr - float3(0, height, 0);

		planes[2].InitWith3Points(ntl, nbl, fbl);
		planes[3].InitWith3Points(ftr, fbr, nbr);
		planes[4].InitWith3Points(ntr, ntl, ftl);
		planes[5].InitWith3Points(fbl, nbl, nbr);

		return true;
	}

	void CCamera::SetTransform(const Vector3f& aPosition, const Vector3f& aRotation)
	{
		SetPosition(aPosition);
		SetRotation(aRotation);
	}

	void CCamera::SetTransform(const Matrix4x4f& aTransform)
	{
		myTransform = aTransform;
	}

	void CCamera::SetRotation(const Vector3f& aRotation)
	{
		Matrix4x4f rotatedMatrix;
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));
		myTransform.SetRotation(rotatedMatrix);
	}

	void CCamera::SetPosition(const Vector3f& aPosition)
	{
		myTransform.SetTranslation(aPosition);
	}

	void CCamera::SetCullingRadius(float aRadius)
	{
		myCullingRadius = aRadius;
	}

	const Vector3f CCamera::GetPosition() const
	{
		return myTransform.GetTranslation();
	}

	const Vector3f CCamera::GetRenderOffset() const
	{
		return myRenderOffset + myShake;
	}

	void CCamera::SetRenderOffset(const Vector3f& anOffset)
	{
		myRenderOffset = anOffset;
	}

	void CCamera::Move(const Vector3f& aMovement)
	{
		Matrix4x4f translation;
		translation.SetTranslation(aMovement);
		translation *= myTransform;
		myTransform = translation;
	}

	void CCamera::Rotate(const Vector3f& aRotation)
	{
		// This rotation is bound by axis

		Matrix4x4f rotatedMatrix(myTransform);
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));
		myTransform.SetRotation(rotatedMatrix);
	}

	void CCamera::GetProjectionPlanes(float& aNearPlane, float& aFarPlane)
	{
		aNearPlane = myNearPlane;
		aFarPlane = myFarPlane;
	}

	void CCamera::SetOrbit(const float2 aRotation, const float2 anOffset, const float aDistance)
	{
		Matrix4x4f identity;

		identity *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		identity *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));

		myTransform.SetRotation(identity);

		float3 dir = { myTransform(3, 1), myTransform(3, 2), myTransform(3, 3) };

		dir *= -aDistance;

		dir += myTransform.GetRight() * anOffset.x;
		dir += myTransform.GetUp() * anOffset.y;

		SetPosition(dir);
	}

	void CCamera::ModFOV(float aMod)
	{
		Init(myFOV + aMod, myRes, myNearPlane, myFarPlane);
	}

	void CCamera::SetFOV(float anFOV)
	{
		Init(anFOV, myRes, myNearPlane, myFarPlane);
	}

	bool CCamera::PassesCulling(const Vector3f& aPosition, float aRadius) const
	{
		Vector3f pos = GetPosition() + GetRenderOffset();
		pos += GetTransform().GetForward() * (myCullingRadius * 0.75f);

		Vector3f distance = aPosition - pos;
		float combinedRadius = aRadius + myCullingRadius;
		return (distance.LengthSqr() <= combinedRadius * combinedRadius);
	}
	
	void CCamera::Shake(float anIntensity, float aDuration)
	{
		if (myShakeIntensity < (anIntensity + 0.00001f))
		{
			myShakeDuration = aDuration;
			myShakeIntensity = anIntensity;
		}
	}

	void CCamera::StopShaking()
	{
		myShakeIntensity = 0.0f;
		myShakeDuration = -0.1f;
	}

	void CCamera::Update(float aDeltaTime)
	{
		if (myShakeDuration > 0.0f)
		{
			float x = myShakeDuration * 1000.f;
			
			myShake.x = myShakeIntensity * sin(x * 0.5f) + cos(x * 3.0f) + cos(sin(x * 4.0f) * 3.0f);
			myShake.z = myShakeIntensity * cos(3.0f * (x - 0.25f)) + sin(-5.0f * x) + abs(sin(x * 3.0f)) * 2.0f - 1.0f;
		}
		else
		{
			myShake.SetAll(0.0f);
			myShakeIntensity = 0.0f;
		}

		myShakeDuration -= aDeltaTime;
	}
}
