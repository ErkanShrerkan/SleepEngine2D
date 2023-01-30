#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_LineDrawer input)
{
    PixelOutput_GBuffer returnValue;
    returnValue.myColor = input.myColor;
    return returnValue;
}