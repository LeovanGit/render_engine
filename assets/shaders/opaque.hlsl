#include "globals.hlsli"
//Texture2D texture0 : register(t0);

struct VSInput
{
    // per-vertex data:
    float3 position : POSITION; // user defined semantics
    float2 uv : UV0;

    // per-instance data:
    float4 transform0 : TRANSFORM0;
    float4 transform1 : TRANSFORM1;
    float4 transform2 : TRANSFORM2;
    float4 transform3 : TRANSFORM3;
};

struct VSOutput
{
    float4 position : SV_POSITION; // system-value semantics
    float2 uv : UV0;
};

VSOutput mainVS(VSInput input)
{
    float4x4 modelMatrix = float4x4(
        input.transform0,
        input.transform1,
        input.transform2,
        input.transform3);

    float4 posWS = mul(float4(input.position.xyz, 1.0f), modelMatrix);
    float4 posCS = mul(g_perView.viewProjMatrix, posWS);

    VSOutput output;
    output.position = posCS;
    output.uv = input.uv;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    //float4 texel = texture0.Sample(linearSampler, input.uv);
    float4 texel = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return texel;
}
