#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    result.myColor.rgb = FullscreenTexture.Sample(pointSampler, input.myUV).rgb;
    result.myColor.a = 1.0;
    return result;
}
