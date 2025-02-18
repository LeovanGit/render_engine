#include "globals.hlsli"

Texture2D<float4> texture0 : register(t0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

VSOutput mainVS(uint index : SV_VertexID)
{
    // generate cube as 6 quad patches in Model Space:
    VSOutput vertices[] = 
    {
        {
            { -1.0f, -1.0f, -1.0f, 1.0f },
            { 0.0f, 0.0f }
        },
        {
            { -1.0f, 1.0f, -1.0f, 1.0f },
            { 0.0f, 1.0f }
        },
        {
            { 1.0f, 1.0f, -1.0f, 1.0f },
            { 1.0f, 1.0f }
        },
        {
            { 1.0f, -1.0f, -1.0f, 1.0f },
            { 1.0f, 0.0f }
        },
        {
            { 1.0f, -1.0f, 1.0f, 1.0f },
            { 0.0f, 0.0f }
        },
        {
            { -1.0f, -1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f }
        },
        {
            { -1.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f }
        },
        {
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f }
        },
    };

    uint indices[] =
    {
        0, 1, 2, 3, // front
        3, 2, 7, 4, // right
        4, 7, 6, 5, // back
        1, 0, 5, 6, // left
        1, 6, 7, 2, // top
        0, 3, 4, 5, // down
    };

    return vertices[indices[index]];
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("mainConstantHS")]
[maxtessfactor(64.0f)]
VSOutput mainHS(
    InputPatch<VSOutput, 4> patch,
    uint index : SV_OutputControlPointID,
    uint patchID : SV_PrimitiveID)
{
    VSOutput output;
    output.position = patch[index].position;
    output.uv = patch[index].uv;

    return output;
}

struct PatchTessFactors
{
    float edgeTessFactors[4] : SV_TessFactor;
    float insideTessFactor[2] : SV_InsideTessFactor;
};

PatchTessFactors mainConstantHS(
    InputPatch<VSOutput, 4> patch,
    uint patchID : SV_PrimitiveID)
{
    float tessFactor = 8;

    PatchTessFactors output;

    output.edgeTessFactors[0] = tessFactor;
    output.edgeTessFactors[1] = tessFactor;
    output.edgeTessFactors[2] = tessFactor;
    output.edgeTessFactors[3] = tessFactor;

    output.insideTessFactor[0] = tessFactor;
    output.insideTessFactor[1] = tessFactor;

    return output;
}

[domain("quad")]
VSOutput mainDS(
    PatchTessFactors patchTessFactors,
    float2 paramCoords : SV_DomainLocation,
    const OutputPatch<VSOutput, 4> patch)
{
    VSOutput output;

    float4 positionMS = lerp(
        lerp(patch[1].position, patch[0].position, paramCoords.y), // along y, left side
        lerp(patch[2].position, patch[3].position, paramCoords.y), // along y, right side
        paramCoords.x); // along x

    // Transfrom cube to quad sphere: set each vertex to unit distance from cube's center.
    // Since we are in Model Space: center is (0, 0, 0) and vector to vertex is equal to its position,
    // so we can just normalize position:
    float4 spherePositionMS = float4(normalize(positionMS.xyz), 1.0f);

    float4 spherePositionWS = mul(g_modelMatrix, spherePositionMS);
    output.position = mul(g_viewProjMatrix, spherePositionWS);

    output.uv = lerp(
        lerp(patch[1].uv, patch[0].uv, paramCoords.y),
        lerp(patch[2].uv, patch[3].uv, paramCoords.y),
        paramCoords.x);

    return output;
}

float4 mainPS(VSOutput input) : SV_Target0
{
    float4 color = texture0.Sample(linearSampler, input.uv);
    
    return color;
}