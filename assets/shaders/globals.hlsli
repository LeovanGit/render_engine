struct PerView
{
    float4x4 viewProjMatrix;
    float3 cameraPostionWS;
    float offset;

    // [0]: top-left
    // [1]: bottom-left
    // [2]: bottom-right
    float4 g_nearPlaneCornersWS[3];
};
ConstantBuffer<PerView> g_perView : register(b0, space0);

//struct PerMesh
//{
//    float4x4 modelMatrix;
//};
//ConstantBuffer<PerMesh> g_perMesh : register(b1);

//sampler linearSampler : register(s0);
