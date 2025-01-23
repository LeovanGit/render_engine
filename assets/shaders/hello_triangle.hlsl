struct VSInput
{
    float3 position : POSITION; // user defined semantics
    float3 color : COLOR0;
};

struct VSOutput
{
    float4 position : SV_POSITION; // system-value semantics
    float3 color : COLOR0;
};

VSOutput mainVS(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position.xyz, 1.0f);
    output.color = input.color;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    return float4(input.color, 1.0f);
}
