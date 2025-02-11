cbuffer PerView : register(b0)
{
    float4x4 g_viewProjMatrix;
    float3 g_cameraPostionWS;
    float g_offset0;
    float3 g_nearPlaneTLCornerWS;
    float g_offset1;
    float3 g_nearPlaneBLCornerWS;
    float g_offset2;
    float3 g_nearPlaneBRCornerWS;
    float g_offset3;
    // float3 g_nearPlaneCorners[3];
};

cbuffer PerMesh : register(b1)
{
    float4x4 g_modelMatrix;
};
