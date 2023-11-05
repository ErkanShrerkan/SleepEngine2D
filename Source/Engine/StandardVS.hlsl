#include "StandardStructs.hlsli"
#include "Functions.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel returnValue;
    
    float4 vertexObjectPosition = input.myPosition;
    
    float4 normal = float4(input.myNormal, 0);
    float4 binormal = float4(input.myBinormal, 0);
    float4 tangent = float4(input.myTangent, 0);
    float4 scale = float4(scale.x, scale.y, scale.z, 0);

    if (hasBones)
    {
        float4 boneWeights = input.myBoneWeights;
        uint4 boneIDs = input.myBoneIDs;
        
        float4x4 boneTransform = mul(boneWeights.x, bones[boneIDs.x]);
        boneTransform += mul(boneWeights.y, bones[boneIDs.y]);
        boneTransform += mul(boneWeights.z, bones[boneIDs.z]);
        boneTransform += mul(boneWeights.w, bones[boneIDs.w]);

        vertexObjectPosition = mul(boneTransform, input.myPosition);
        normal = mul(boneTransform, normal);
        binormal = mul(boneTransform, binormal);
        tangent = mul(boneTransform, tangent);
    }

    float4 vertexWorldPosition = mul(toWorld, vertexObjectPosition);

    float4 vertexViewPosition = mul(toCamera, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(toProjection, vertexViewPosition);

    float3x3 toWorldRotation = (float3x3) toWorld;
    float3 vertexWorldNormal = mul(toWorldRotation, normal.xyz);
    float3 vertexWorldTangent = mul(toWorldRotation, tangent.xyz);
    float3 vertexWorldBinormal = mul(toWorldRotation, binormal.xyz);
    
    returnValue.myPosition = vertexProjectionPosition;
    returnValue.myColor = input.myColor;
    returnValue.myNormal = vertexWorldNormal;
    returnValue.myTangent = vertexWorldTangent;
    returnValue.myBinormal = vertexWorldBinormal;
    returnValue.myUV = input.myUV;
    returnValue.myWorldPosition = vertexWorldPosition;
    returnValue.myDepth = vertexProjectionPosition.z > 0 ? vertexProjectionPosition.z / vertexProjectionPosition.w : 0;
    return returnValue;
}
