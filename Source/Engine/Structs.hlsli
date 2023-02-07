
static const float GaussianKernel5[5] =
{
    0.06136, 0.24477, 0.38774, 0.24477, 0.06136
};

static const float2 SampleOffsets[] =
{
    { -1, -1 },
    { 0, -1 },
    { 1, -1 },
    { -1, 0 },
    { 1, 0 },
    { -1, 1 },
    { 0, 1 },
    { 1, 1 }
};

struct VertexInput_LineDrawer
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float mySpace : SPACE;
};

struct VertexInput_Fullscreen
{
    unsigned int myIndex : SV_VertexID;
    unsigned int myID : SV_InstanceID;
    float4 myPosition : POSITION;
    float2 myUV : UV;
};

struct VertexToPixel_Fullscreen
{
    float4 myPosition : SV_POSITION;
    float2 myUV : UV;
};

struct PixelOutput_Fullscreen
{
    float4 myColor : SV_TARGET;
};

struct VertexInput_GBuffer
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

struct QuadInput_GBuffer
{
    float4 myPosition : POSITION;
    float2 myUV : UV;
    
    float3 myRight : RIGHT;
    float3 myUp : UP;
    float3 myForward : FORWARD;
    float3 myPos : OFFSEST;
    float3 myColor : COLOR;
    float4 myRect : RECT;
};

struct VertexToPixel_LineDrawer
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
};

struct VertexToPixel_GBuffer
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

struct PixelOutput_GBuffer
{
    float4 myColor : SV_TARGET;
};

struct VertexInput_Sprite
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct VertexToPixel_Sprite
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float2 myRectUV : RECTUV;
};

struct PixelOutput_Sprite
{
    float4 myColor : SV_TARGET;
};

cbuffer PostProcessingBuffer_Fullscreen : register(b3)
{
    float3 PPBD_shallowWaterColor;
    float PPBD_waterLevel;
    float3 PPBD_deepWaterColor;
    float PPBD_refractionIndex;
    float3 PPBD_waterBorderColor;
    float PPBD_refractionStrength;
    uint2 PPBD_screenResolution;
    uint2 PPBD_gameResolution;
    float PPBD_borderThreshhold;
    float PPBD_TimePI;
    float PPBD_DeltaTime;
    float PPBD_fogDensity;
    float3 PPBD_fogColor;
    float PPBD_fogMult;
    float PPBD_fogOffset;
    float PPBD_godRaySampleDistance;
    float PPBD_godRayFalloff;
    float PPBD_godRayScatterProbability;
    float PPBD_alphaThreshold;
    int PPBD_config;
    float PPBD_nearPlane;
    float PPBD_elapsedTime;
    float PPBD_emissiveMult;
    float PPBD_bloomMult;
    int PPBD_volumetricIndex;
    float PPBD_y;
}

cbuffer SpriteObjectBuffer_Sprite : register(b0)
{
    float4x4 SOB_Transform;
    float4 SOB_Color;
    float4 SOB_Rect;
    float2 SOB_Size;
    float2 SOB_PosOffset;
    float2 SOB_Pivot;
    float SOB_Rotation;
    float SOB_Data;
}

cbuffer FrameBufferData_Sprite : register(b1)
{
    float4x4 myToCamera;
    float4x4 myCameraTransform;
    float4x4 myToProjection;
    float4 myCameraPosition;
};

// effect textures
Texture2D FullscreenTexture : register(t0);
Texture2D FullscreenTexture2 : register(t1);

// sprite textures
Texture2D spriteTexture : register(t0);
Texture2D maskTexture : register(t1);

// samplers
SamplerState defaultSampler : register(s0);
SamplerState wrapSampler : register(s1);
SamplerState pointSampler : register(s2);
SamplerState aniFilSampler : register(s3);