#include "globals.hlsli"
Texture2D texture0 : register(t0);

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

struct HSOutput
{
    float4 position : POSITION;
    float2 uv : UV0;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
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

    float d0 = 1.0f;
    float d1 = 5.0f;

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

    float4 position = patch[0].position * barycentric.x +
        patch[1].position * barycentric.y +
        patch[2].position * barycentric.z;

    output.position = mul(g_viewProjMatrix, position);

    output.uv = patch[0].uv * barycentric.x +
        patch[1].uv * barycentric.y +
        patch[2].uv * barycentric.z;
	
    return output;
}

float4 mainPS(DSOutput input) : SV_TARGET0
{
    float4 texel = texture0.Sample(linearSampler, input.uv);
    return texel;
}
