sampler linearSampler : register(s0);
Texture2D texture0 : register(t0);

cbuffer PerView : register(b0)
{
    float4x4 viewProjMatrix;
};

cbuffer PerMesh : register(b1)
{
    float4x4 modelMatrix;
};

struct VSInput
{
    float3 position : POSITION; // user defined semantics
    float2 uv : UV0;
};

struct VSOutput
{
    float4 position : SV_POSITION; // system-value semantics
    float2 uv : UV0;
};

VSOutput mainVS(VSInput input)
{
    float4 posWS = mul(modelMatrix, float4(input.position.xyz, 1.0f));
    float4 posCS = mul(viewProjMatrix, posWS);

    VSOutput output;
    output.position = posCS;
    output.uv = input.uv;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    float4 texel = texture0.Sample(linearSampler, input.uv);
    return texel;
}
