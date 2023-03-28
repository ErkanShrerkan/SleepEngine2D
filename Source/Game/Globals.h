#pragma once
#include "pch.h"
#include <Engine\FullscreenTexture.h>

struct GlobalSettings
{
public:
	uint2 gameplayResolution = { 640, /*480*/ 360 };
	uint2 windowResolution = { 1920, 1080 };
	float4 gameWindowRect = { 0, 0, 1, 1 };
	float4 windowRect = { 0, 0, 1, 1 };
	SE::CFullscreenTexture* gameViewTexture;
};