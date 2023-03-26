#include "pch.h"
#include "PointLight.h"
#include "Engine\Engine.h"
#include "Engine\ContentLoader.h"

namespace SE
{
    CPointLight::CPointLight()
    {
        unsigned y = 16384 / 4;
        myShadowTextures.resize(4);
        for (int i = 0; i < 2; i++)
        {
            myShadowTextures[i] = Singleton<CTextureFactory>().CreateShadowMap({y, y});
        }
    }

    void CPointLight::SetPosition(const Vector3f& aPosition)
    {
        myPosition.x = aPosition.x;
        myPosition.y = aPosition.y;
        myPosition.z = aPosition.z;
    }
    void CPointLight::SetColor(const Vector3f& aColor)
    {
        //float mod = aColor.Length();
        myColor.r = aColor.r; //+ .1f * mod;
        myColor.g = aColor.g; //+ .1f * mod;
        myColor.b = aColor.b; //+ .1f * mod;
        //myColor.Normalize();
    }
    void CPointLight::SetRange(float aRange)
    {
        myPosition.w = aRange;
    }
    void CPointLight::SetIntensity(float anIntensity)
    {
        myColor.a = anIntensity;
    }
    const Vector4f& CPointLight::GetBundledPosition() const noexcept
    {
        return myPosition;
    }
    const Vector4f& CPointLight::GetBundledColor() const noexcept
    {
        return myColor;
    }
    Vector4f& CPointLight::GetRawBundledPosition()
    {
        return myPosition;
    }
    Vector4f& CPointLight::GetRawBundledColor()
    {
        return myColor;
    }
}
