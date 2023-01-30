#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    float4 sampleC = FullscreenTexture.Sample(defaultSampler, input.myUV).rgba;

    float3 color = float3(0, 0, 0);
    float width = 4;
    float2 pixel = { width / PPBD_screenResolution.x, width / PPBD_screenResolution.y };

    for (int i = 0; i < 8; i++)
    {
        float2 uv = float2(input.myUV.x + (pixel.x * SampleOffsets[i].x), input.myUV.y + (pixel.y * SampleOffsets[i].y));
        color += FullscreenTexture.Sample(defaultSampler, uv).rgb;
    }

    returnValue.myColor.rgb = color / 8;
    returnValue.myColor.a = sampleC.a;
    return returnValue;
}