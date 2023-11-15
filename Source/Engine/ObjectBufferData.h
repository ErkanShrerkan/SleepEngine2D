#pragma once

struct ObjectBufferData
{
	Matrix4x4f myToWorld;
	unsigned int myHasBones;
	unsigned int myNumBones;
	int x, y; // padding
	Matrix4x4f myBones[128];
};