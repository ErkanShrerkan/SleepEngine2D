#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_LineDrawer main(VertexInput_LineDrawer input)
{
    VertexToPixel_LineDrawer returnValue;
    
    float2 pos = input.myPosition.xy;
    pos = pos * 2 - 1;
    pos.y *= -1;

    returnValue.myPosition = float4(pos.x, pos.y, .5, 1);
    returnValue.myColor = input.myColor;
    return returnValue;
}
