struct VS_INPUT
{
    float3 posWS : POSITION;
};

struct VS_OUTPUT
{
    float4 posCS : SV_POSITION;
};

VS_OUTPUT vertexShader(VS_INPUT input)
{
    VS_OUTPUT output;
    output.posCS = float4(input.posWS, 1.0f);

    return output;
}

float4 pixelShader(VS_OUTPUT input) : SV_TARGET
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
