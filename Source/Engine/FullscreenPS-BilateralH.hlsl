#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
	
    float texelSize = 1. / (PPBD_screenResolution.x / 4.);
    float3 blurColor = float3(0, 0, 0);
    float normalizationFactor = 0.0f;
    float bZ = 1. / normpdf(0., SIGMA);
    float colorFactor = 0.;
    float3 originalPixelValue = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    
    unsigned int kernelSize = 5;
    float start = (((float) (kernelSize) - 1.) / 2.) * -1.;
    
    for (unsigned int i = 0; i < kernelSize; i++)
    {
        float2 uv = input.myUV + float2(texelSize * (start + float(i)), 0);
        float3 resource = FullscreenTexture.Sample(defaultSampler, uv).rgb;
        colorFactor = normpdf3(resource - originalPixelValue, SIGMA) * bZ * GaussianKernel5[i];
        normalizationFactor += colorFactor;
        blurColor += resource * colorFactor;
    }
	
    returnValue.myColor.rgb = blurColor / normalizationFactor;
    returnValue.myColor.a = 1.;
    return returnValue; 
}