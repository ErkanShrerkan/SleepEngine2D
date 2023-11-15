#pragma once

struct FrameBufferData
{
	Matrix4x4f myCameraTransform;
	Matrix4x4f myToCamera;
	Matrix4x4f myToProjection;
	Vector4f myCameraPosition;
};