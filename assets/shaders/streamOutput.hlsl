struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput mainVS(VSInput input)
{
    VSOutput output;
    output.position = input.position;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    return float4(0.0f, 0.46f, 0.79f, 1.0f);
}
