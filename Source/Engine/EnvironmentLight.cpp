#include "pch.h"
#include "EnvironmentLight.h"
#include "Engine\Engine.h"
#include "Engine\ContentLoader.h"
#include "DX11.h"

#include "Engine\CDebugDrawer.h"

namespace SE
{
    CEnvironmentLight::CEnvironmentLight()
    {
        unsigned y = 16384 / 2;
        myShadowTextures.resize(2);
        for (int i = 0; i < 2; i++)
        {
            myShadowTextures[i] = CEngine::GetInstance()->GetContentLoader()->GetTextureFactory().CreateShadowMap({ y, y });
        }
    }
    CEnvironmentLight::~CEnvironmentLight()
    {
        for (auto& tex : myShadowTextures)
        {
            tex.Release();
        }
        myCubemap->Release();
    }
    void CEnvironmentLight::SetDirection(const Vector3f& aDirection)
    {
        myDirection = Vector4f(aDirection, 0.f);
    }
    void CEnvironmentLight::SetRotation(const Vector3f& aRotation)
    {
        Matrix4x4f identity;

        identity *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
        identity *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
        identity *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));

        myTransform.SetRotation(identity);

        float3 dir = myTransform.GetForward();
        myDirection = { dir.x, dir.y, dir.z, 0 };
        dir *= -10000.f;

        myShadowCam->SetRotation(aRotation);
        myShadowCam->SetPosition(dir);
    }
    void CEnvironmentLight::SetDirectionalColor(const Vector4f& aColor)
    {
        myDirectionalColor.r = aColor.r;
        myDirectionalColor.g = aColor.g;
        myDirectionalColor.b = aColor.b;
        myDirectionalColor.a = aColor.a;
    }
    void CEnvironmentLight::SetAmbientColor(const Vector4f& aColor)
    {
        myAmbientColor = aColor;
    }
    ID3D11ShaderResourceView* const& CEnvironmentLight::GetCubeMap() const
    {
        return myCubemap;
    }
    Vector4f& CEnvironmentLight::GetRawDirectionalColor()
    {
        return myDirectionalColor;
    }
    void CEnvironmentLight::AddProjection(Matrix4x4f aProj)
    {
        myCamProjections.push_back(aProj);
    }
    const Vector4f& CEnvironmentLight::GetDirectionalColor() const
    {
        return myDirectionalColor;
    }
    const Vector4f& CEnvironmentLight::GetAmbientColor() const
    {
        return myAmbientColor;
    }
    const Vector4f& CEnvironmentLight::GetDirection() const
    {
        return myDirection;
    }
    const float& CEnvironmentLight::GetMipLevels() const
    {
        return myMipLevels;
    }
    Vector4f& CEnvironmentLight::GetRawAmbient()
    {
        return myAmbientColor;
    }
}
