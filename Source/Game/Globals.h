#pragma once
#include "pch.h"
#include <Engine\FullscreenTexture.h>

struct GlobalSettings
{
public:
	float4 GetGameWindowNormalised();
	float4 GetGameScreenNormalised();
	float4 GetWindowNormalised();
	float2 GetGameWindowCenterPixel();
	float2 GetWindowCenterPixel();

public:
	uint2 gameplayResolution = { 640, /*480*/ 360 };
	uint2 windowResolution = { 1920, 1080 };
	uint2 screenResolution = { 1920, 1080 };
	float4 gameWindowRect = { 16, 16, 32, 32 }; // window space
	float4 gameScreenRect = { 16, 16, 32, 32 }; // screen space
	float4 gameViewRect = { 0, 0, 1, 1 }; // uv of game texture shown in game window
	float4 windowRect = { 16, 16, 32, 32 };
	SE::CFullscreenTexture* gameViewTexture;
};