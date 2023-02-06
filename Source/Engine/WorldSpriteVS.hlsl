#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_Sprite main(VertexInput_Sprite input)
{
    VertexToPixel_Sprite returnValue;

    float2 uv = input.myUV;
	float4 vertexObjectPosition = input.myPosition;
    float4 vertexWorldPosition = mul(SOB_Transform, vertexObjectPosition);
    float4 vertexViewPosition = mul(myToCamera, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(myToProjection, vertexViewPosition);

    returnValue.myPosition = float4(vertexProjectionPosition.x, vertexProjectionPosition.y, .5, 1);
    returnValue.myColor = input.myColor;
    
    returnValue.myRectUV = uv;
    uv.x = Remap(uv.x, 0, 1, SOB_Rect.x, SOB_Rect.z);
    uv.y = Remap(uv.y, 0, 1, SOB_Rect.y, SOB_Rect.w);
    returnValue.myUV = uv;
    
    return returnValue;
}
