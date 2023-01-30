#pragma once
#include "Camera.h"
#include "FullscreenTexture.h"

namespace SE
{
    class CPointLight
    {
    public:
        CPointLight();
        void SetPosition(const Vector3f& aPosition);
        void SetColor(const Vector3f& aColor);
        void SetRange(float aRange);
        void SetIntensity(float anIntensity);
        void SetCastsShadows(bool aValue) { myCastsShadows = aValue; }

        bool GetCastsShadows() { return myCastsShadows; }
        const Vector4f& GetBundledPosition() const noexcept;
        const Vector4f& GetBundledColor() const noexcept;
        Vector4f& GetRawBundledPosition();
        Vector4f& GetRawBundledColor();

        CCamera* GetShadowCam() { return myShadowCam; }
        std::vector<CFullscreenTexture>& GetShadowTextures() { return myShadowTextures; }
        const std::vector<Matrix4x4f>& GetShadowCamTransforms() { return myCamTransforms; }

    private:
        // XYZ - Position
        // W - Range
        Vector4f myPosition;
        // RGB - Color
        // A - Intensity
        Vector4f myColor;
        bool myCastsShadows = true;

        CCamera* myShadowCam;
        std::vector<CFullscreenTexture> myShadowTextures;
        std::vector<Matrix4x4f> myCamTransforms;
    };
}
