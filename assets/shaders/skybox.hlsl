#include "globals.hlsli"

TextureCube skyboxTexture : register(t0);
sampler linearSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 sampleDirection : DIRECTION;
};

VSOutput mainVS(uint vertexID : SV_VertexID)
{
    float2 positionsCS[] =
    {
        { -1.0f, -1.0f },
        { -1.0f, 3.0f },
        { 3.0f, -1.0f }
    };

    float3 bottomLeftDirWS = g_nearPlaneBLCornerWS - g_cameraPostionWS;
    float3 dx = g_nearPlaneBRCornerWS - g_nearPlaneBLCornerWS;
    float3 dy = g_nearPlaneTLCornerWS - g_nearPlaneBLCornerWS;

    float2 uv = (positionsCS[vertexID] + float2(1.0f, 1.0f)) / 2.0f;

    VSOutput output;
    output.position = float4(positionsCS[vertexID], 0.0f, 1.0f); // reversed depth: .z = 0.0f, far plane
    output.sampleDirection = bottomLeftDirWS + dx * uv.x + dy * uv.y;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    return skyboxTexture.Sample(linearSampler, input.sampleDirection);
}