#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float4 resource = FullscreenTexture.Sample(defaultSampler, input.myUV).rgba;
    float4 color;

    float brightness = dot(resource.rgb, float3(0.2126, 0.7152, 0.0722));
    float luminance = sqrt((.299 * resource.r * resource.r) + (.587 * resource.g * resource.g) + (.114 * resource.b * resource.b));
    if (luminance > .5)
        color = float4(resource.rgb, 1.0);
    else
        color = float4(0.0, 0.0, 0.0, 0.0);
    
    result.myColor = color;
    return result;
}