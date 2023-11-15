#include "pch.h"
#include "CameraSystem.h"
#include "GameManager.h"
#include "CameraComponent.h"
#include <Engine\FrameBufferData.h>
#include "Transform.h"
#include "Entity.h"

void CameraSystem::Update()
{
	bool hasUpdatedFrameBuffer = false;

	for (auto& [entity, camera] : myGameManager->GetComponentMap<CameraComponent>().map)
	{
		camera.UpdateProjection();

		// TODO: only do this to main camera
		if (hasUpdatedFrameBuffer)
		{
			continue;
		}

		FrameBufferData& fbd = Singleton<FrameBufferData>();

		Transform& camTransform = *myGameManager->GetComponent<Transform>(entity);

		fbd.myCameraPosition.xyz = camTransform.GetPosition();
		fbd.myCameraPosition.w = 1.f;
		fbd.myCameraTransform = camTransform.GetTransform();
		fbd.myToCamera = float4x4::GetFastInverse(fbd.myCameraTransform);
		fbd.myToProjection = camera.GetProjection();

		hasUpdatedFrameBuffer = true;
	}
}
