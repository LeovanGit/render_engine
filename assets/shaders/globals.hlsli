cbuffer PerView : register(b0)
{
    float3 g_cameraPostion;
    float g_offset;
    float4x4 g_viewProjMatrix;
};

cbuffer PerMesh : register(b1)
{
    float4x4 g_modelMatrix;
};
