#pragma once

struct ObjectBufferData
{
	Matrix4x4f myToWorld;
	float4 myScale;
	unsigned int myHasBones;
	unsigned int myNumBones;
	int x, y;
	Matrix4x4f myBones[128];
	float4 myColor;
};