#pragma once
#include "pch.h"

struct GlobalSettings
{
public:
	uint2 gameplayResolution = { 640, /*480*/ 360 };
	float4 gameWindowRect = { 0, 0, 1, 1 };
	bool isEditingMode = true;
};