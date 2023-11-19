#include "Structs.hlsli"
#include "StandardStructs.hlsli"
#include "Functions.hlsli"

VertexToPixel_LineDrawer main(VertexInput_LineDrawer input)
{
    VertexToPixel_LineDrawer returnValue;
    
    float4 pos = float4(0, 0, 0, 1);
    if(input.mySpace < .5)
    {
        float4 vertexWorldPosition = input.myPosition;
        float4 vertexViewPosition = mul(toCamera, vertexWorldPosition);
        pos = mul(toProjection, vertexViewPosition);
        returnValue.myPosition = pos;
    }
    else
    {
        pos = input.myPosition;
        pos = pos * 2 - 1;
        pos.y *= -1;
        returnValue.myPosition = float4(pos.x, pos.y, .5, 1);
    }

    returnValue.myColor = input.myColor;
    return returnValue;
}
