#include "globals.hlsli"

sampler linearSampler : register(s0);
Texture2D texture0 : register(t0);

struct VSInput
{
    float3 position : POSITION;
    float2 uv : UV0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
};

struct HSOutput
{
    float4 position : POSITION;
    float2 uv : UV0;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
    float3 color : COLOR;
};

VSOutput mainVS(VSInput input)
{
    float4 posWS = mul(g_modelMatrix, float4(input.position.xyz, 1.0f));

    VSOutput output;
    output.position = posWS;
    output.uv = input.uv;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("mainConstantHS")]
[maxtessfactor(64.0f)]
HSOutput mainHS(InputPatch<VSOutput, 3> patch,
		   uint i : SV_OutputControlPointID,
		   uint patchID : SV_PrimitiveID)
{
    HSOutput output;
    output.position = patch[i].position;
    output.uv = patch[i].uv;

    return output;
}

struct PatchTessFactors
{
    float EdgeTessFactors[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

PatchTessFactors mainConstantHS(InputPatch<VSOutput, 3> patch,
					 uint patchID : SV_PrimitiveID)
{
    float4 triangleCenterWS = (patch[0].position + patch[1].position + patch[2].position) / 3;
    float d = distance(triangleCenterWS.xyz, g_cameraPostionWS.xyz);

    float d0 = 20.0f;
    float d1 = 50.0f;

    // d > d1 -> tessFactor is 1
    // d < d0 -> tessFactor is 64 (max possible)
    // d in [d0; d1] -> tessFactor in [1; 64]
    float tessFactor = 63.0f * saturate((d1 - d) / (d1 - d0)) + 1.0f;

    PatchTessFactors output;
    output.EdgeTessFactors[0] = tessFactor; // u
    output.EdgeTessFactors[1] = tessFactor; // v
    output.EdgeTessFactors[2] = tessFactor; // w
    output.InsideTessFactor = tessFactor;

    return output;
}

[domain("tri")]
DSOutput mainDS(PatchTessFactors patchTessFactors,
			 float3 barycentric : SV_DomainLocation,
			 const OutputPatch<HSOutput, 3> patch)
{
    DSOutput output;

    output.uv = patch[0].uv * barycentric.x +
        patch[1].uv * barycentric.y +
        patch[2].uv * barycentric.z;

    float4 position = patch[0].position * barycentric.x +
        patch[1].position * barycentric.y +
        patch[2].position * barycentric.z;
    
    // calculate patch normal:
    float3 v1 = patch[1].position.xyz - patch[0].position.xyz;
    float3 v2 = patch[2].position.xyz - patch[0].position.xyz;
    float3 normal = normalize(cross(v1, v2));

    // Sample() is not available in DS
    // .r .g .b store the same value, because heightmap is single channel texture
    float height = texture0.SampleLevel(linearSampler, output.uv, 0).r;
    float power = 8.0f;
    position += float4(normal.xyz * height * power, 0.0f);
    output.position = mul(g_viewProjMatrix, position);

    const uint biomesCount = 5;
    float3 biomeColor[biomesCount] =
    {
        float3(0.0f, 0.0f, 0.6f), // ocean
        float3(0.745f, 0.72f, 0.42f), // beach
        float3(0.0f, 0.4f, 0.0f), // forest
        float3(0.57f, 0.3f, 0.1f), // mountains
        float3(1.0f, 1.0f, 1.0f), // snow
    };

    float biome = (biomesCount - 1) * height; // [0; 4]
    uint biomeIndex = floor(biome);

    output.color = lerp(biomeColor[biomeIndex], biomeColor[biomeIndex + 1], frac(biome));

    return output;
}

float4 mainPS(DSOutput input) : SV_TARGET0
{
    return float4(input.color, 1.0f);
}
