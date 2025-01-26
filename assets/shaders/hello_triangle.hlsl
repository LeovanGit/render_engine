sampler linearSampler : register(s0);
Texture2D texture0 : register(t0);

struct VSInput
{
    float3 position : POSITION; // user defined semantics
    float3 color : COLOR0;
    float2 uv : UV0;
};

struct VSOutput
{
    float4 position : SV_POSITION; // system-value semantics
    float3 color : COLOR0;
    float2 uv : UV0;
};

VSOutput mainVS(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position.xyz, 1.0f);
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    float4 texel = texture0.Sample(linearSampler, input.uv);
    return texel;
}
