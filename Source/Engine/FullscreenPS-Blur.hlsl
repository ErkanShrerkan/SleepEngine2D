#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    float4 sample = FullscreenTexture.Sample(defaultSampler, input.myUV).rgba;

    float3 color = sample;
    float width = 7.5;
    float luminance = color.r * 0.3 + color.g * 0.59 + color.b * 0.11; //sqrt((.299 * sample.r * sample.r) + (.587 * sample.g * sample.g) + (.114 * sample.b * sample.b));
    luminance *= 1 - clamp(PPBD_bloomMult / 1000.f, 0, 1);
    float luMod = clamp(1.f / luminance, 0, .75);
    float2 pixel = { width * luMod / 1920.f, width * luMod / 1080.f };

    for (int i = 0; i < 8; i++)
    {
        float2 uv = float2(input.myUV.x + (pixel.x * SampleOffsets[i].x), input.myUV.y + (pixel.y * SampleOffsets[i].y));
        color += FullscreenTexture.Sample(defaultSampler, uv).rgb;
    }

    returnValue.myColor.rgb = color / 9;
    returnValue.myColor.a = sample.a;
    return returnValue;
}