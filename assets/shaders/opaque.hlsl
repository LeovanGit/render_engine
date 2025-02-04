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

struct GSOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
};

VSOutput mainVS(VSInput input)
{
    float4 posWS = mul(modelMatrix, float4(input.position.xyz, 1.0f));

    VSOutput output;
    output.position = posWS;
    output.uv = input.uv;

    return output;
}

[maxvertexcount(3)]
void mainGS(triangle VSOutput input[3],
    inout TriangleStream<GSOutput> output)
{
    float3 v1 = input[1].position.xyz - input[0].position.xyz;
    float3 v2 = input[2].position.xyz - input[0].position.xyz;
    float3 normal = cross(v1, v2);

    // move triangle (each vertex) along normal:
    float dt = 0.05f;
    for (uint i = 0; i != 3; ++i)
    {
        float4 newPosWS = float4(input[i].position.xyz + normal * dt, input[i].position.w);
        float4 newPosCS = mul(viewProjMatrix, newPosWS);

        GSOutput vertex;
        vertex.position = newPosCS;
        vertex.uv = input[i].uv;

        output.Append(vertex);
    }

    output.RestartStrip();
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    float4 texel = texture0.Sample(linearSampler, input.uv);
    return texel;
}
