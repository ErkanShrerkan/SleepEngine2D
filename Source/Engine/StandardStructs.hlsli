struct VertexInput
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float3 myNormal : NORMAL;
    float3 myTangent : TANGENT;
    float3 myBinormal : BINORMAL;
    uint4 myBoneIDs : BONEIDS;
    float4 myBoneWeights : BONEWEIGHTS;
    float4 myRight : RIGHT;
    float4 myUp : UP;
    float4 myForward : FORWARD;
    float4 myPos : POS;
};

struct VertexToPixel
{
    float4 myPosition : SV_POSITION;
    float4 myWorldPosition : WORLD_POSITION;
    float4 myColor : COLOR;
    float3 myNormal : NORMAL;
    float3 myTangent : TANGENT;
    float3 myBinormal : BINORMAL;
    float2 myUV : UV;
    float myDepth : DEPTH;
};

cbuffer FrameBuffer : register(b0)
{
    float4x4 cameraTransform;
    float4x4 toCamera;
    float4x4 toProjection;
    float4 cameraPosition;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 toWorld;
    float4 scale;
    unsigned int hasBones;
    unsigned int numBones;
    int xx, yy;
    float4x4 bones[128];
    float4 color;
}

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2);