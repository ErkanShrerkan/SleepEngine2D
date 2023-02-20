#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_LineDrawer main(VertexInput_LineDrawer input)
{
    VertexToPixel_LineDrawer returnValue;
    
    float2 pos = float2(0, 0);
    if(input.mySpace > .5)
    {
        float4 vertexWorldPosition = input.myPosition;
        float4 vertexViewPosition = mul(myToCamera, vertexWorldPosition);
        pos = mul(myToProjection, vertexViewPosition);
    }
    else
    {
        pos = input.myPosition.xy;
        pos = pos * 2 - 1;
        pos.y *= -1;
    }

    returnValue.myPosition = float4(pos.x, pos.y, .5, 1);
    returnValue.myColor = input.myColor;
    return returnValue;
}
