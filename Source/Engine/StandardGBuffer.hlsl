#include "Functions.hlsli"
#include "StandardStructs.hlsli"
#include "Samplers.hlsli"

struct GBufferOutput
{
    float4 myWorldPosition : SV_TARGET0;
    float4 myAlbedo : SV_TARGET1;
    float4 myNormal : SV_TARGET2;
    float4 myVertexNormal : SV_TARGET3;
    float3 myMaterial : SV_TARGET4;
    float myAmbientOcclusion : SV_TARGET5;
    float myDepth : SV_TARGET6;
};

Texture2D albedoTexture : register(t8);
Texture2D normalTexture : register(t9);
Texture2D materialTexture : register(t10);

GBufferOutput main(VertexToPixel input)
{
    float2 scaledUV = input.myUV;
    float4 albedo = albedoTexture.Sample(aniFilSampler, scaledUV).rgba;
    //albedo.rgb = GammaToLinear(albedo.rgb);
    if (albedo.a < 0.95 || dot((cameraPosition.xyz - input.myWorldPosition.xyz), input.myNormal.xyz) < 0)
    {
        discard;
    }
    float3 normal = normalTexture.Sample(aniFilSampler, scaledUV).wyz;
    float ambientOcclusion = normal.b;
    
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1.f - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = (normal); // ingen saturate
    
    float3x3 TBN = float3x3(
        normalize(input.myTangent.xyz),
        normalize(input.myBinormal.xyz),
        normalize(input.myNormal.xyz)
    );
    
    TBN = transpose(TBN);
    
    float3 pixelNormal = normalize(mul(TBN, normal));
    float3 material = materialTexture.Sample(aniFilSampler, scaledUV).rgb;
    
    float4 viewPosition = mul(toCamera, input.myWorldPosition);
    float4 projectionPosition = mul(toProjection, viewPosition);
    
    GBufferOutput output;
    output.myWorldPosition = input.myWorldPosition;
    output.myAlbedo = albedo;
    output.myNormal = float4(pixelNormal, 1);
    output.myVertexNormal = float4(input.myNormal.xyz, 1);
    output.myMaterial = material;
    output.myAmbientOcclusion = ambientOcclusion;
    output.myDepth = projectionPosition.z / projectionPosition.w;
    
    return output;
}