#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    result.myColor.rgba = FullscreenTexture.Sample(pointSampler, input.myUV).rgba;
    //result.myColor.a = 1.0;
    return result;
}
