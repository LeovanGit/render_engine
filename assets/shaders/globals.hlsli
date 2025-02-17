cbuffer PerView : register(b0)
{
    float4x4 g_viewProjMatrix;
    float3 g_cameraPostionWS;
    float g_offset;

    // [0]: top-left
    // [1]: bottom-left
    // [2]: bottom-right
    float4 g_nearPlaneCornersWS[3];
};

cbuffer PerMesh : register(b1)
{
    float4x4 g_modelMatrix;
};

sampler linearSampler : register(s0);
