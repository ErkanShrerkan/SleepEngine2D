#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_Sprite main(VertexInput_Sprite input)
{
    VertexToPixel_Sprite returnValue;

	float4 vertexObjectPosition = input.myPosition;
    float4 vertexWorldPosition = mul(myTransform, vertexObjectPosition);
    float4 vertexViewPosition = mul(myToCamera, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(myToProjection, vertexViewPosition);

    returnValue.myPosition = float4(vertexProjectionPosition.x, vertexProjectionPosition.y * -1, .5, 1);
    //returnValue.myPosition = float4(vertexObjectPosition.x, vertexObjectPosition.y, .5, 1);
	returnValue.myColor = input.myColor;
    
    returnValue.myRectUV = input.myUV;
    float2 uv = input.myUV;
    uv.x = Remap(uv.x, 0, 1, myRect.x, myRect.z);
    uv.y = Remap(uv.y, 0, 1, myRect.y, myRect.w);
    returnValue.myUV = uv;
    
    return returnValue;
}
