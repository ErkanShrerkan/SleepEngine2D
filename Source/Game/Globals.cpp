#include "pch.h"
#include "Globals.h"

float4 GlobalSettings::GetGameWindowNormalised()
{
    float4 normalised =
    {
         gameWindowRect.x / (float)screenResolution.x,
         gameWindowRect.y / (float)screenResolution.y,
         gameWindowRect.z / (float)screenResolution.x,
         gameWindowRect.w / (float)screenResolution.y
    };
    return normalised;
}

float4 GlobalSettings::GetGameScreenNormalised()
{
    float4 normalised =
    {
         gameScreenRect.x / (float)screenResolution.x,
         gameScreenRect.y / (float)screenResolution.y,
         gameScreenRect.z / (float)screenResolution.x,
         gameScreenRect.w / (float)screenResolution.y
    };
    return normalised;
}

float4 GlobalSettings::GetWindowNormalised()
{
    float4 normalised =
    {
        windowRect.x / (float)screenResolution.x,
        windowRect.y / (float)screenResolution.y,
        windowRect.z / (float)screenResolution.x,
        windowRect.w / (float)screenResolution.y
    };
    return normalised;
}

float2 GlobalSettings::GetGameWindowCenterPixel()
{
    return    
    {
        .5f * (gameWindowRect.x + gameWindowRect.z) + windowRect.x,
        .5f * (gameWindowRect.y + gameWindowRect.w) + windowRect.y
    };
}

float2 GlobalSettings::GetWindowCenterPixel()
{
    return    
    {
        .5f * (windowRect.x + windowRect.z),
        .5f * (windowRect.y + windowRect.w)
    };
}
