#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_Sprite main(VertexInput_Sprite input)
{
    VertexToPixel_Sprite returnValue;
    
    float2 size = SOB_Size;
    float rads = SOB_Rotation;// / 180 * PI;
    float2 offset = float2(.5 - SOB_Pivot.x, .5f - SOB_Pivot.y);
    
    float2x2 rotation =
    {
        cos(rads), -sin(rads) * (9.f / 16),
        sin(rads) * (16.f / 9), cos(rads),
    };
    
    float2 pos = input.myPosition.xy;
    pos *= size;
    pos = mul(rotation, pos);
    offset = mul(rotation, offset * size);
    pos += offset;
    pos += SOB_PosOffset.xy;
    pos = pos * 2 - 1;
    //pos.y *= -1;
    
    returnValue.myPosition = float4(pos.x, -pos.y, .5, 1);
    returnValue.myColor = input.myColor;
    
    returnValue.myRectUV = input.myUV;
    float2 uv = input.myUV;
    uv.x = Remap(uv.x, 0, 1, SOB_Rect.x, SOB_Rect.z);
    uv.y = 1 - Remap(uv.y, 0, 1, SOB_Rect.y, SOB_Rect.w);
    returnValue.myUV = uv;
    
    return returnValue;
}
