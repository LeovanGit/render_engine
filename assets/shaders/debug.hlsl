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
    float3 position : POSITION;
    float2 uv : UV0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput mainVS(VSInput input)
{
    float4 posWS = mul(modelMatrix, float4(input.position.xyz, 1.0f));
    
    VSOutput output;
    output.position = posWS;

    return output;
}

// input triangle and output 3 lines - normals for 3 vertices
[maxvertexcount(6)]
void mainGS(triangle VSOutput input[3],
    inout LineStream<VSOutput> output)
{
    float3 v1 = input[1].position.xyz - input[0].position.xyz;
    float3 v2 = input[2].position.xyz - input[0].position.xyz;
    float3 normal = normalize(cross(v1, v2));

    float normalLength = 0.25f;

    for (uint i = 0; i != 3; ++i)
    {
        VSOutput vertexA;
        vertexA.position = mul(viewProjMatrix, input[i].position);

        VSOutput vertexB;
        vertexB.position = mul(viewProjMatrix, input[i].position + normalLength * float4(normal, 0.0f));
        
        output.Append(vertexA);
        output.Append(vertexB);
        output.RestartStrip();
    }
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    return float4(0.0f, 0.46f, 0.79f, 1.0f);
}