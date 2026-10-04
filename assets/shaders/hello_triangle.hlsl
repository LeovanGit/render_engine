sampler linearSampler : register(s0);
Texture2D texture0 : register(t0);

cbuffer PerView : register(b0)
{
    float4x4 viewProjMatrix;
    float4 cameraPos;
};

cbuffer PerMesh : register(b1)
{
    float4x4 modelMatrix;
    float4x4 modelMatrixInv;
};

struct VSInput
{
    float3 position : POSITION; // user defined semantics
    float2 uv : UV0;
};

struct VSOutput
{
    float4 position : SV_POSITION; // system-value semantics
    float3 positionBB : POSITION;
    float2 uv : UV0;
};

VSOutput mainVS(VSInput input)
{
    float4 posWS = mul(modelMatrix, float4(input.position.xyz, 1.0f));
    float4 posCS = mul(viewProjMatrix, posWS);

    VSOutput output;
    output.position = posCS;
    output.positionBB = input.position;
    output.uv = input.uv;

    return output;
}

float4 mainPS(VSOutput input) : SV_TARGET0
{
    // our box from VB, but with "infinite" height (y),
    // to avoid ray intersections with box top side:
    float3 minBB = float3(-1, -100, -1);
    float3 maxBB = float3(1, 100, 1);
    
    // ray with box intersection in object space (Slab method):
    float3 cameraPosBB = mul(modelMatrixInv, float4(cameraPos.xyz, 1.0f));
    float3 rayCamToPixBB = normalize(input.positionBB.xyz - cameraPosBB.xyz);
    
    float3 tnear = (minBB - cameraPosBB) / rayCamToPixBB;
    float3 tfar = (maxBB - cameraPosBB) / rayCamToPixBB;
    
    float3 tmin = min(tnear, tfar);
    float3 tmax = max(tnear, tfar);
    
    float tStart = max(tmin.x, max(tmin.y, tmin.z));
    float tEnd = min(tmax.x, min(tmax.y, tmax.z));
    
    if (tStart > tEnd)
    {
        float tmp = tStart;
        tStart = tEnd;
        tEnd = tmp;
    }
    
    // we have 100% intersection, because of ray from camera to cur. pixel,
    // so we can skip this:
    // if (tEnd > 0)
    
    float4 startCoord = float4(cameraPosBB + tStart * rayCamToPixBB, 1.0f);
    float4 endCoord = float4(cameraPosBB + tEnd * rayCamToPixBB, 1.0f);
    
    // UVW coords relative to realMinBB (so UVW = (0, 0, 0) in realMinBB coords):
    float3 realMinBB = float3(-1, -1, -1);
    float3 realMaxBB = float3(1, 1, 1);
    
    float3 volumeSizes = realMaxBB - realMinBB; // width, height, depth of box
    
    float3 startUVW = (startCoord.xyz - realMinBB) / volumeSizes;
    float3 endUVW = (endCoord.xyz - realMinBB) / volumeSizes;
   
    if (endUVW.z >= 0.999f || endUVW.z <= 0.001f)
        return texture0.Sample(linearSampler, float2(endUVW.x, endUVW.y * 5));
    else if (endUVW.x >= 0.999f || endUVW.x <= 0.001f)
        return texture0.Sample(linearSampler, float2(endUVW.z, endUVW.y * 5));
    else
        return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
