#include "Structs.hlsli"

Texture2D textureToCopy : register(t0);

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    result.myColor.rgba = textureToCopy.Sample(pointSampler, input.myUV).rgba;
    //result.myColor.a = 1.0;
    return result;
}
