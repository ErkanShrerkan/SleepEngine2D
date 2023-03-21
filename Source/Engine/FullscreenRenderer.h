#pragma once
#include <array>
struct ID3D11VertexShader;
struct ID3D11PixelShader;
namespace SE
{
    class CFullscreenRenderer
    {
    public:
        enum EShader : uint
        {
            EShader_Copy,
            EShader_Luminance,
            EShader_GaussianHorizontal,
            EShader_GaussianVertical,
            EShader_Bloom,
            EShader_Border,
            EShader_ToCamera,
            EShader_WaterUniversal,
            EShader_WaterMerge,
            EShader_DebugSpheres,
            EShader_Fog,
            EShader_AlphaBlend,
            EShader_AdditiveBlend,
            EShader_Outline,
            EShader_Highlight,
            EShader_Glass,
            EShader_Blur,
            EShader_Tonemap,
            EShader_LuminanceBlur,
            EShader_DirectionalLightSmear,
            EShader_BilateralFilter,
            EShader_AverageBlur,
            EShader_VolumetricMerge,
            EShader_BilateralH,
            EShader_BilateralV,
            EShader_ToRawColor,
            EShader_GammaToLinear,
            EShader_LinearToGamma,


            // Keep last, don't remove
            EShader_Count
        };
        ~CFullscreenRenderer();
        bool Init();
        void Render(const EShader& aMode);
        ID3D11VertexShader* GetVertexShader() { return myVertexShader; }
        ID3D11PixelShader* GetPixelShader(EShader aShader) { return myPixelShaders[aShader]; }

    private:
        struct SFullscreenDataBuffer
        {
            float2 myResolution;
            float myElapsedTime;
        }
        myDataBuffer;

        ID3D11VertexShader* myVertexShader;
        std::array<ID3D11PixelShader*, EShader_Count> myPixelShaders;
    };
}
